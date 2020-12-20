#pragma once

#include <regex>

#include "renderer/types.hpp"

#include "renderer/netrender/httplib/http_request.hpp"
#include "renderer/netrender/httplib/tcp_client.hpp"

namespace Oxy::NetRender::HTTP {

  struct URI {
    std::string scheme;
    std::string userinfo;
    std::string host;
    int         port = 0;
    std::string path;
    std::string query;
    std::string fragment;
  };

  inline std::optional<URI> parse_uri(const std::string& uri) {
    static std::regex uri_regex("^(([^:/?#]+):)?(//([^/?#]*))?([^?#]*)(\\?([^#]*))?(#(.*))?");

    std::smatch matches;

    if (std::regex_search(uri, matches, uri_regex)) {
      std::string host     = matches[4].str();
      std::string userinfo = "";
      int         port     = 80;

      // regex doesnt fully work, needs some manual massaging

      if (auto pos = host.find('@'); pos != std::string::npos) {
        userinfo = host.substr(0, pos);
        host     = host.substr(pos + 1);
      }

      if (auto pos = host.find(':'); pos != std::string::npos) {
        std::stringstream ss;
        ss << host.substr(pos + 1);
        ss >> port;

        host = host.substr(0, pos);
      }

      URI result;

      result.scheme   = matches[2];
      result.userinfo = userinfo;
      result.host     = host;
      result.port     = port;
      result.path     = matches[5];
      result.query    = matches[7];
      result.fragment = matches[9];

      return result;
    }

    return {};
  }

  using ResponseCallback = std::function<void()>;

  class HTTPClient {
  public:
    explicit HTTPClient(Method method, const std::string& uri)
        : m_uri(parse_uri(uri).value_or(URI{}))
        , m_request()
        , m_tcp_client(m_uri.host, m_uri.port) {

      m_request.set_method(method);
      m_request.set_path(m_uri.path);
    }

    void on_response(ResponseCallback callback) {
      m_callback = callback;
      m_tcp_client.on_receive([this](const char*, std::size_t) {
        if (m_callback)
          m_callback();
      });
    }

    void set_header(const std::string& name, const std::string& content) {
      m_request.set_header(name, content);
    }

    void set_body(const std::string& body) { m_request.set_body(body); }

    void append_body(const std::string& content) { m_request.append_body(content); }

    void send() {
      auto req = m_request.format_into_request_str();
      std::cout << req << "\n";
      m_tcp_client.send(req.c_str(), req.size());
    }

    void block() { m_tcp_client.block(); }

  public:
    REF(request);

  private:
    URI            m_uri;
    HTTPRequest    m_request;
    TCP::TCPClient m_tcp_client;

    ResponseCallback m_callback;
  };

} // namespace Oxy::NetRender::HTTP
