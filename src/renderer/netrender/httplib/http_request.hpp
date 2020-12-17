#pragma once

#include <optional>
#include <string>

#include "renderer/netrender/httplib/tcp_server.hpp"

#include "renderer/netrender/httplib/http_header.hpp"
#include "renderer/netrender/httplib/http_response.hpp"

namespace Oxy::NetRender::HTTP {

  std::vector<std::string> split(const std::string& str, const std::string& delim, bool keep_empty);

  class HTTPRequest {
  public:
    HTTPRequest(const void* request, std::size_t len);

    auto& method() const { return m_method; }
    auto  method_enum() const { return m_method_enum; };
    auto& uri() const { return m_uri; }
    auto& query_str() const { return m_query_string; }
    auto& http_version() const { return m_http_version; }
    auto& body() const { return m_request_body; }
    auto  failed_parse() const { return m_parse_failed; }

    HTTPHeader header(const std::string& name) const;

    std::size_t num_headers() const { return m_request_headers.size(); }

  private:
    bool parse(const void* request, std::size_t len);

  private:
    bool m_parse_failed;

    std::string m_method;
    Method      m_method_enum;

    std::string m_uri;
    std::string m_query_string;
    std::string m_http_version;

    std::unordered_map<std::string, HTTPHeader> m_request_headers;
    std::string                                 m_request_body;
  };

} // namespace Oxy::NetRender::HTTP
