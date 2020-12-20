#pragma once

#include <chrono>
#include <deque>
#include <functional>
#include <mutex>
#include <optional>
#include <regex>
#include <thread>
#include <vector>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

#include "renderer/types.hpp"

namespace Oxy::NetRender::TCP {

  namespace ch = std::chrono;

  using ClientReceiveCallback = std::function<void(const char*, size_t)>;

  enum class ClientState {
    CONNECTING,
    CONNECTED,
    CLOSING,
    CLOSED,
    ERROR,
  };

  class TCPClient {
  public:
    TCPClient(const std::string& ip_or_hostname, int port);

    void block() {
      if (m_client_thread.joinable())
        m_client_thread.join();
    }

    void send(const char* data, std::size_t size) {
      std::lock_guard g{m_to_be_sent_mtx};

      for (std::size_t i = 0; i < size; i++)
        m_to_be_sent.push_back(data[i]);
    }

    void close() { m_state = ClientState::CLOSING; }

    void on_receive(ClientReceiveCallback callback) { m_receive_callback = callback; }

    CREF(state);

  private:
    ClientState m_state;

    ClientReceiveCallback m_receive_callback;

    std::thread m_client_thread;

    std::mutex       m_to_be_sent_mtx;
    std::deque<char> m_to_be_sent;

    char* m_send_buf;
    char* m_recv_buf;

    int         m_fd;
    std::string m_ip;
  };

} // namespace Oxy::NetRender::TCP
