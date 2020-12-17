#include "renderer/netrender/httplib/endpoint.hpp"

namespace Oxy::NetRender::HTTP {

  Endpoint::Endpoint(const std::string& name)
      : m_is_valid(true)
      , m_name(name)
      , m_method(Method::GET) {}

  void Endpoint::call(const HTTPRequest& req, HTTPResponse& res) const { m_functor(req, res); }

  void Endpoint::set_functor(Method method, EndpointFunctor functor) {
    m_method  = method;
    m_functor = functor;
  }

  Namespace::Namespace(const std::string& name)
      : m_name(name) {}

  Namespace::~Namespace() {
    for (auto& [path, endpoint] : m_subnamespaces)
      delete endpoint;

    m_subnamespaces.clear();
  }

  Endpoint Namespace::match(const std::vector<std::string>& path, std::size_t pos) {

    if (pos == path.size() - 1 && path.back() == m_name) {
      if (m_endpoint.has_value())
        return m_endpoint.value();
      else
        return Endpoint::null();
    }

    auto& first = path[pos];

    if (first == m_name) {
      for (auto& [_, subnamespace] : m_subnamespaces) {
        if (auto endpoint = subnamespace->match(path, pos + 1); endpoint.is_valid())
          return endpoint;
      }
    }

    return Endpoint::null();
  }

  bool Namespace::has_subnamespace(const std::string& name) {
    return m_subnamespaces.contains(name.c_str());
  }

  Namespace* Namespace::get_subnamespace(const std::string& name) {
    return m_subnamespaces.at(name.c_str());
  }

  void Namespace::add_subnamespace(const std::string& name, Namespace* ns) {
    auto cname = name.c_str();

    if (m_subnamespaces.contains(cname)) {
      delete m_subnamespaces[cname];
      m_subnamespaces.erase(cname);
    }

    m_subnamespaces[cname] = ns;
  }

  void Namespace::set_endpoint(Method method, EndpointFunctor functor) {
    if (!m_endpoint.has_value()) {
      m_endpoint = Endpoint("/");
    }

    m_endpoint.value().set_functor(method, functor);
  }

} // namespace Oxy::NetRender::HTTP
