#pragma once

#include <filesystem>
#include <mutex>
#include <unordered_map>

#include "renderer/netrender/httplib/http_common.hpp"

#include "renderer/netrender/httplib/http_request.hpp"
#include "renderer/netrender/httplib/http_response.hpp"

#include "renderer/netrender/httplib/endpoint.hpp"
#include "renderer/netrender/httplib/tcp_server.hpp"

#include "renderer/netrender/httplib/http_session.hpp"

namespace Oxy::NetRender::HTTP {

  namespace fs = std::filesystem;

  struct HTTPServerConf {
    ssize_t max_request_size = 16 * 1024 * 1024;
    float   timeout          = 10;
  };

  class HTTPServer {
  public:
    HTTPServer(int port, const HTTPServerConf& conf = {});

    constexpr auto http_version() { return "HTTP/1.1"; }
    constexpr auto server_version() { return "OxyNetRender/0.1"; }

    auto port() const { return m_tcp_server.port(); }

    void handle_connection(TCP::Connection* client);
    void handle_request(TCP::Connection* client, const void* data, ssize_t len);

    void set_404_handler(HTTP::EndpointFunctor functor);

    void get(const std::string& path, EndpointFunctor functor);
    void post(const std::string& path, EndpointFunctor functor);

    void block();

    static std::vector<std::string> split_uri(const std::string& uri);

  private:
    void push_endpoint(Method method, const std::string& path, EndpointFunctor functor);

  private:
    HTTPServerConf m_conf;

    std::unordered_map<TCP::Connection*, HTTPSession*> m_sessions;
    std::mutex                                         m_sessions_mtx;

    std::vector<fs::path> m_public_search_paths;

    HTTP::EndpointFunctor m_404_handler;

    TCP::TCPServer m_tcp_server;

    Namespace m_root_namespace;
  };

} // namespace Oxy::NetRender::HTTP
