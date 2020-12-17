#include "renderer/netrender/httplib/tcp_connection.hpp"

namespace Oxy::NetRender::TCP {

  Connection::Connection(int fd)
      : m_state(ConnectionState::OPEN)
      , m_close_reason(CloseReason::NORMAL)
      , m_ready_for_receive(false)
      , m_fd(fd)
      , m_fd_closed(false)
      , m_buf_dirty(false)
      , m_total_received(0)
      , m_timeout(10.0)
      , m_last_activity(ch::system_clock::now())
      , m_start_time(ch::system_clock::now())
      , m_receive_callback([](auto, auto, auto) {})
      , m_closed_callback([](auto) {}) {

    m_buf = new char[CLIENT_BUFFER_SIZE];
    memset(m_buf, 0, CLIENT_BUFFER_SIZE);

    m_read_thread = std::thread([this] {
      while (m_state == ConnectionState::OPEN) {
        if (!m_ready_for_receive) {
          std::this_thread::yield();
          continue;
        }

        if (!alive()) {
          m_state        = ConnectionState::DEAD;
          m_close_reason = CloseReason::ERRORED;
          break;
        }

        if (did_timeout()) {
          m_state        = ConnectionState::DEAD;
          m_close_reason = CloseReason::TIMEOUT;
          break;
        }

        clear_buf();
        auto res = maybe_read();

        if (res == -1) {
          if (handle_errno()) {
            m_state        = ConnectionState::DEAD;
            m_close_reason = CloseReason::ERRORED;
            break;
          }
        }
        else if (res > 0) {
          m_receive_callback(this, (const void*)m_buf, res);
        }

        std::this_thread::yield();
      }

      // close_async or close_sync was called
      if (m_state == ConnectionState::CLOSING)
        m_close_reason = CloseReason::NORMAL;

      if (m_state != ConnectionState::DEAD)
        m_state = ConnectionState::CLOSED;

      close_fd();
      m_closed_callback(m_close_reason);
    });
  }

  Connection::~Connection() {
    if (m_state != ConnectionState::DEAD)
      m_state = ConnectionState::CLOSED;

    delete[] m_buf;

    if (m_read_thread.joinable())
      m_read_thread.join();

    close_fd();
  }

  void Connection::on_receive(ReceiveCallback cb) { m_receive_callback = cb; }

  void Connection::on_closed(ClosedCallback cb) { m_closed_callback = cb; }

  void Connection::set_timeout(double timeout) { m_timeout = timeout; }

  bool Connection::write(const void* data, std::size_t len) {
    if (!alive())
      return false;

    return send(m_fd, data, len, 0) == (ssize_t)len;
  }

  void Connection::close_async() { m_state = ConnectionState::CLOSING; }

  void Connection::close_sync() {
    m_state = ConnectionState::CLOSING;
    if (m_read_thread.joinable())
      m_read_thread.join();
  }

  bool Connection::alive() const { return fcntl(m_fd, F_GETFL) != -1; }

  double Connection::average_bytes_per_second() const {
    auto now     = ch::system_clock::now();
    auto dur     = ch::duration_cast<ch::microseconds>(now - m_start_time);
    auto seconds = dur.count() / 1e6;

    return (double)m_total_received / seconds;
  }

  void Connection::clear_buf() {
    if (m_buf_dirty) {
      memset(m_buf, 0, CLIENT_BUFFER_SIZE);
      m_buf_dirty = false;
    }
  }

  int Connection::maybe_read() {
    auto res = read(m_fd, m_buf, CLIENT_BUFFER_SIZE);
    if (res > 0) {
      m_buf_dirty     = true;
      m_last_activity = ch::system_clock::now();

      m_total_received += res;

      return res;
    }

    return -1;
  }

  bool Connection::handle_errno() {
    switch (errno) {
    case EBADF:
    case EFAULT:
    case EIO: return true;
    case EWOULDBLOCK: return false;
    default: return false;
    }
  }

  bool Connection::did_timeout() const {
    auto now = ch::system_clock::now();
    auto dur = ch::duration(now - m_last_activity);

    using namespace std::chrono_literals;
    return dur > 1s * m_timeout;
  }

  void Connection::close_fd() {
    if (!m_fd_closed) {
      shutdown(m_fd, SHUT_RDWR);
      close(m_fd);

      m_fd_closed = true;
    }
  }

} // namespace Oxy::NetRender::TCP
