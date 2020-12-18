#include "renderer/netrender/httplib/tcp_server.hpp"

namespace Oxy::NetRender::TCP {

  std::string get_uuid() {
    static std::random_device dev;
    static std::mt19937       rng(dev());

    std::uniform_int_distribution<int> dist(0, 15);

    constexpr const char* v      = "0123456789abcdef";
    constexpr bool        crap[] = {0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0};

    std::string res;
    res.reserve(64);

    for (int i = 0; i < 16; i++) {
      if (crap[i])
        res += "-";
      res += v[dist(rng)];
      res += v[dist(rng)];
    }

    return res;
  }

  TCPServer::TCPServer(int port)
      : m_port(port)
      , m_state(ServerState::RUNNING)
      , m_init_error(std::nullopt)
      , m_opt(1)
      , m_connection_callback([](auto) {})
      , m_last_healthcheck(ch::system_clock::now()) {

    if (m_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0); m_fd < 0) {
      m_init_error = "socket() failed";
      m_state      = ServerState::CLOSED;
      return;
    }

    if (setsockopt(m_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &m_opt, sizeof(m_opt)) < 0) {
      m_init_error = "setsockopt() failed";
      m_state      = ServerState::CLOSED;
      return;
    }

    m_address.sin_family      = AF_INET;
    m_address.sin_addr.s_addr = INADDR_ANY;
    m_address.sin_port        = htons(port);

    if (bind(m_fd, (sockaddr*)&m_address, sizeof(m_address)) < 0) {
      m_init_error = "bind() failed";
      m_state      = ServerState::CLOSED;
      return;
    }

    if (listen(m_fd, 25) < 0) {
      m_init_error = "listen() failed";
      return;
    }
  }

  TCPServer::~TCPServer() {
    if (m_listener_thread.joinable())
      m_listener_thread.join();

    std::lock_guard g(m_connections_mtx);

    for (auto& connection : m_connections)
      connection->close_sync();

    for (auto& connection : m_connections)
      delete connection;

    m_connections.clear();

    shutdown(m_fd, SHUT_RDWR);
    close(m_fd);
  }

  void TCPServer::on_connection(ConnectionCallback cb) { m_connection_callback = cb; }

  void TCPServer::start_listen() {
    m_listener_thread = std::thread([this] {
      while (m_state == ServerState::RUNNING) {
        int addrlen = sizeof(m_address);
        int newsock = accept(m_fd, (sockaddr*)&m_address, (socklen_t*)&addrlen);

        if (newsock >= 0) {
          auto uuid = get_uuid();

          // set nonblocking flag
          int flags = fcntl(newsock, F_GETFL, 0);
          fcntl(newsock, F_SETFL, flags | O_NONBLOCK);

          std::lock_guard g(m_connections_mtx);
          auto            connection = new Connection(newsock);

          m_connections.push_back(connection);
          m_connection_callback(connection);

          connection->set_ready();
        }

        // healthcheck
        auto now = ch::system_clock::now();
        auto dur = ch::duration(now - m_last_healthcheck);

        using namespace std::chrono_literals;
        if (dur > 1s) {
          m_last_healthcheck = now;

          std::lock_guard g(m_connections_mtx);

          auto it =
              std::remove_if(m_connections.begin(), m_connections.end(), [&](auto connection) {
                if (!connection->alive()) {
                  delete connection;
                  return true;
                }
                return false;
              });

          m_connections.erase(it, m_connections.end());
        }

        std::this_thread::yield();
      }
    });
  }

  void TCPServer::stop_listen() { m_state = ServerState::CLOSING; }

  void TCPServer::block() {
    if (m_listener_thread.joinable())
      m_listener_thread.join();
  }

} // namespace Oxy::NetRender::TCP
