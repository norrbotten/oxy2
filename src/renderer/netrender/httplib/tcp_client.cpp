#include "renderer/netrender/httplib/tcp_client.hpp"

namespace Oxy::NetRender::TCP {

  std::string resolve_hostname(const std::string& hostname) {
    hostent* he;

    if (he = gethostbyname(hostname.c_str()); he == NULL)
      return "";

    in_addr** addr_list = (in_addr**)he->h_addr_list;

    for (int i = 0; addr_list[i] != NULL; i++)
      return std::string(inet_ntoa(*addr_list[i]));

    return "";
  }

  TCPClient::TCPClient(const std::string& ip_or_hostname, int port = 80) {
    static std::regex match_ipv4("^(?:[0-9]{1,3}\\.){3}[0-9]{1,3}$");

    m_client_thread = std::thread([this, ip = ip_or_hostname, port = port] {
      m_state = ClientState::CONNECTING;

      if (std::regex_match(ip, match_ipv4))
        m_ip = ip;
      else {
        auto resolve = resolve_hostname(ip);
        std::cout << resolve << "\n";

        if (resolve == "") {
          m_state = ClientState::ERROR;
          return;
        }

        m_ip = resolve;
      }

      if (m_fd = socket(AF_INET, SOCK_STREAM, 0); m_fd < 0) {
        m_state = ClientState::ERROR;
        std::cout << "socket() bork\n";
        return;
      }

      std::cout << port << "\n";

      sockaddr_in server_addr;
      bzero(&server_addr, sizeof(server_addr));

      server_addr.sin_family = AF_INET;
      server_addr.sin_port   = htons(port);
      inet_aton(m_ip.c_str(), &server_addr.sin_addr);

      // set nonblocking flag
      fcntl(m_fd, F_SETFL, fcntl(m_fd, F_GETFL) | O_NONBLOCK);

      if (connect(m_fd, (sockaddr*)&server_addr, sizeof(server_addr)) != 0 &&
          errno != EINPROGRESS) {
        m_state = ClientState::ERROR;
        return;
      }

      m_state = ClientState::CONNECTED;

      m_send_buf = new char[16384]();
      m_recv_buf = new char[16384]();

      while (m_state == ClientState::CONNECTED) {

        if (m_to_be_sent.size() > 0) {
          // i am pretty sure this is safe...

          std::lock_guard g{m_to_be_sent_mtx};

          auto len = std::min((std::size_t)16384, m_to_be_sent.size());
          for (decltype(len) i = 0; i < len; i++)
            m_send_buf[i] = m_to_be_sent[i];

          m_to_be_sent.erase(m_to_be_sent.begin(), m_to_be_sent.begin() + len);

          if (::send(m_fd, m_send_buf, len, 0) < 0 && errno != EWOULDBLOCK) {
            m_state = ClientState::ERROR;
          }
        }

        auto numread = recv(m_fd, m_recv_buf, sizeof(m_recv_buf), 0);

        if (numread == -1) {
          if (errno != EWOULDBLOCK) {
            std::cout << strerror(errno) << "\n";
            m_state = ClientState::ERROR;
          }
        }
        else {
          if (m_receive_callback)
            m_receive_callback(m_recv_buf, numread);
        }

        /*
        if (auto numread = recv(m_fd, m_recv_buf, sizeof(m_recv_buf), 0);
            numread < 0 && errno != EWOULDBLOCK) {

          m_state = ClientState::ERROR;
        }
        else {
          if (errno != EWOULDBLOCK && m_receive_callback)
            m_receive_callback(m_recv_buf, numread);
        }
        */

        std::this_thread::yield(); // breath my dear
      }

      delete[] m_send_buf;
      delete[] m_recv_buf;

      ::close(m_fd);

      if (m_state == ClientState::CLOSING) {
        m_state = ClientState::CLOSED;
      }
    });
  }

} // namespace Oxy::NetRender::TCP
