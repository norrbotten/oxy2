#pragma once

#include <functional>
#include <thread>

#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

namespace Oxy::NetRender::TCP {

  namespace ch = std::chrono;

  enum class ConnectionState {
    OPEN,
    CLOSING,
    CLOSED,
    DEAD,
  };

  enum class CloseReason {
    NORMAL,
    TIMEOUT,
    ERRORED,
  };

  class Connection;

  using ReceiveCallback = std::function<void(Connection*, const void*, std::size_t)>;
  using ClosedCallback  = std::function<void(CloseReason)>;

  constexpr std::size_t CLIENT_BUFFER_SIZE = 16 * 1024;

  class Connection final {
  public:
    explicit Connection(int fd);
    ~Connection();

    /*
        Sets the connection to be ready for receiving.
    */
    void set_ready() { m_ready_for_receive = true; }

    /*
        Sets the recieve callback, the callback will be called immediately when
        the client has sent a chunk of data. The maximum size of the data is
        CLIENT_BUFFER_SIZE - 1.
    */
    void on_receive(ReceiveCallback cb);

    /*
        Sets the closed callback, the callback will be called after
        close(...) has been called on the file descriptor.
    */
    void on_closed(ClosedCallback cb);

    // Sets the timeout in seconds
    void set_timeout(double timeout);

    /*
        Writes len bytes from data to the client
        Returns true on success, false on failure
    */
    bool write(const void* data, std::size_t len);

    // Closes the client asyncrounously
    void close_async();

    /*
        Closes the client syncrounously, ie. this function doesnt return until the
        thread has shut down, it calls join on the read thread
    */
    void close_sync();

    // Returns true if the client connection is alive
    bool alive() const;

    double average_bytes_per_second() const;

    auto fd() const { return m_fd; }
    auto state() const { return m_state; }

  private:
    // Clears the internal buffer
    void clear_buf();

    // Performs read(...) and returns its return value
    int maybe_read();

    // Handles errors from read(...), returns true if errored
    bool handle_errno();

    // Checks if the client has timed out
    bool did_timeout() const;

    // Closes the fd, if it already hasnt been
    void close_fd();

  private:
    ConnectionState m_state;
    CloseReason     m_close_reason;

    bool m_ready_for_receive;

    int  m_fd;
    bool m_fd_closed;

    char* m_buf;
    bool  m_buf_dirty;

    std::size_t m_total_received;

    double m_timeout;

    std::thread m_read_thread;

    ch::system_clock::time_point m_last_activity;
    ch::system_clock::time_point m_start_time;

    ReceiveCallback m_receive_callback;
    ClosedCallback  m_closed_callback;
  };

} // namespace Oxy::NetRender::TCP
