#pragma once

#include "renderer/types.hpp"

#include "renderer/netrender/httplib/http_request.hpp"

namespace Oxy::NetRender::HTTP {

  class HTTPClient {
    explicit HTTPClient(HTTP::Method method, const std::string& uri)
        : m_request() {

      m_request.set_method(method);
      m_request.set_uri(uri);
    }

    void set_header(const std::string& name, const std::string& content) {
      m_request.set_header(name, content);
    }

    void set_body(const std::string& body) { m_request.set_body(body); }

    void append_body(const std::string& content) { m_request.append_body(content); }

  public:
    REF(request);

  private:
    HTTPRequest m_request;
  };

} // namespace Oxy::NetRender::HTTP
