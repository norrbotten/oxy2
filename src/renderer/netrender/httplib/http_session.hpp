#pragma once

#include <optional>

#include "renderer/netrender/httplib/tcp_connection.hpp"

#include "renderer/netrender/httplib/http_request.hpp"
#include "renderer/netrender/httplib/http_response.hpp"

#include "renderer/netrender/httplib/endpoint.hpp"

namespace Oxy::NetRender::HTTP {

  class HTTPSession {
  public:
    HTTPSession(TCP::Connection* connection);

    bool done() const { return m_done; }

    void force_done() { m_done = true; }

    void handle_request(const Endpoint& endpoint, const HTTPRequest& req, HTTPResponse& res);

  private:
    bool m_done;

    TCP::Connection* m_connection;
  };

} // namespace Oxy::NetRender::HTTP
