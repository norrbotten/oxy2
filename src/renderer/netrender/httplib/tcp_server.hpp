#pragma once

#include <algorithm>
#include <chrono>
#include <mutex>
#include <optional>
#include <random>
#include <vector>

#include "renderer/netrender/httplib/tcp_connection.hpp"

namespace Oxy::NetRender::TCP {

  namespace ch = std::chrono;

  std::string get_uuid();

  enum class ServerState {
    RUNNING,
    CLOSING,
    CLOSED,
  };

  using ConnectionCallback = std::function<void(TCP::Connection*)>;

  class TCPServer final {
  public:
    explicit TCPServer(int port);
    ~TCPServer();

    void on_connection(ConnectionCallback cb);

    void start_listen();
    void stop_listen();

    auto init_errored() const { return m_init_error.has_value(); }
    auto init_error() const { return m_init_error.value_or(""); }

    void block();

    auto port() const { return m_port; }

  private:
    int m_port;

    ServerState m_state;

    std::optional<std::string> m_init_error;

    int         m_fd;
    int         m_opt;
    sockaddr_in m_address;

    std::thread m_listener_thread;

    ConnectionCallback m_connection_callback;

    ch::system_clock::time_point m_last_healthcheck;

    std::vector<TCP::Connection*> m_connections;
    std::mutex                    m_connections_mtx;
  };

} // namespace Oxy::NetRender::TCP
