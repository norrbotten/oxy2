#include "renderer/netrender/httplib/http_server.hpp"

#include <iostream>

namespace Oxy::NetRender::HTTP {

  HTTPServer::HTTPServer(int port, const HTTPServerConf& conf)
      : m_conf(conf)
      , m_tcp_server(port)
      , m_root_namespace("") {

    m_tcp_server.start_listen();

    if (m_tcp_server.init_errored()) {
      std::cout << m_tcp_server.init_error() << "\n";
    }
    else {
      m_tcp_server.on_connection(
          [&](TCP::Connection* connection) { this->handle_connection(connection); });
    }
  }

  void HTTPServer::handle_connection(TCP::Connection* connection) {
    connection->on_closed([&](TCP::CloseReason) {
      std::lock_guard g(m_sessions_mtx);
      if (m_sessions.contains(connection))
        m_sessions.erase(connection);
    });

    connection->on_receive([&](TCP::Connection* con, const void* data, ssize_t len) {
      this->handle_request(con, data, len);
    });
  }

  void HTTPServer::handle_request(TCP::Connection* connection, const void* data, ssize_t len) {
    HTTPRequest  req(data, len);
    HTTPResponse res(connection);

    res.write_header("Server", "OxyNetRender/0.1");

    HTTPSession* session = nullptr;

    if (m_sessions.contains(connection))
      session = m_sessions[connection];
    else {
      session = new HTTPSession(connection);

      std::lock_guard g(m_sessions_mtx);
      m_sessions[connection] = session;
    }

    if (req.failed_parse()) {
      res.status(400).finalize().send();
      session->force_done();
    }
    else {
      auto endpoint = m_root_namespace.match(split_uri(req.uri()));

      if (!endpoint.is_valid()) {
        if (m_404_handler) {
          m_404_handler(req, res);
          session->force_done();
        }
      }
      else {
        session->handle_request(endpoint, req, res);
      }
    }

    if (session->done()) {
      connection->close_async();

      std::lock_guard g(m_sessions_mtx);
      m_sessions.erase(connection);
    }
  }

  void HTTPServer::set_404_handler(HTTP::EndpointFunctor functor) { m_404_handler = functor; }

  void HTTPServer::get(const std::string& path, EndpointFunctor functor) {
    push_endpoint(Method::GET, path, functor);
  }

  void HTTPServer::post(const std::string& path, EndpointFunctor functor) {
    push_endpoint(Method::POST, path, functor);
  }

  void HTTPServer::block() { m_tcp_server.block(); }

  std::vector<std::string> HTTPServer::split_uri(const std::string& uri) {
    auto cpy = uri;

    // trim
    cpy.erase(cpy.begin(), std::find_if(cpy.begin(), cpy.end(),
                                        std::not1(std::ptr_fun<int, int>(std::isspace))));

    // wtf clang-format, thats not very nice
    cpy.erase(
        std::find_if(cpy.rbegin(), cpy.rend(), std::not1(std::ptr_fun<int, int>(std::isspace)))
            .base(),
        cpy.end());

    if (cpy[0] != '/')
      cpy = "/" + cpy;

    auto path_segments = split(cpy, "/", true);

    return path_segments;
  }

  void HTTPServer::push_endpoint(Method method, const std::string& path, EndpointFunctor functor) {
    auto path_segments = split_uri(path);

    auto ns = &this->m_root_namespace;

    for (std::size_t i = 1; i < path_segments.size(); i++) {
      auto& name = path_segments[i];

      if (ns->has_subnamespace(name)) {
        ns = ns->get_subnamespace(name);
      }
      else {
        auto subnamespace = new Namespace(name);
        ns->add_subnamespace(name, subnamespace);

        ns = subnamespace;
      }
    }

    ns->set_endpoint(method, functor);
  }

} // namespace Oxy::NetRender::HTTP
