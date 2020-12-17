#pragma once

#include <optional>
#include <vector>

#include "renderer/netrender/httplib/http_common.hpp"
#include "renderer/netrender/httplib/tcp_connection.hpp"

#include "renderer/netrender/httplib/http_header.hpp"

namespace Oxy::NetRender::HTTP {

  class ResponseAlreadyFinalized : public std::exception {
  public:
    ResponseAlreadyFinalized()
        : m_what("Response is already finalized") {}

    const char* what() noexcept { return m_what.c_str(); }

  private:
    std::string m_what;
  };

  class HTTPResponse {
  public:
    HTTPResponse(TCP::Connection* connection);

    void send();
    void strip_body();

    void unfinalize() { m_finalized = false; }

    HTTPResponse& write_header(const std::string& name, const std::string& payload);
    HTTPResponse& write_body(const std::string& payload);
    HTTPResponse& status(ssize_t status);
    HTTPResponse& finalize();

    HTTPHeader  header(const std::string& name) const;
    std::size_t num_headers() const { return m_response_headers.size(); }

    std::size_t body_length() const { return m_response_body.length(); }
    bool        has_body() const { return body_length() > 0; }

    auto finalized() const { return m_finalized; }
    auto sent() const { return m_sent; }

  private:
    TCP::Connection* m_connection;

    bool m_finalized;
    bool m_sent;

    status_code m_status;

    std::unordered_map<std::string, HTTPHeader> m_response_headers;
    std::string                                 m_response_body;
  };

} // namespace Oxy::NetRender::HTTP
