#pragma once

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "renderer/netrender/httplib/http_common.hpp"

#include "renderer/netrender/httplib/http_request.hpp"
#include "renderer/netrender/httplib/http_response.hpp"

namespace Oxy::NetRender::HTTP {

  typedef std::function<void(const HTTPRequest&, HTTPResponse&)> EndpointFunctor;

  class Endpoint {
  public:
    Endpoint(const std::string& name);

    auto name() const { return m_name; }
    auto method() const { return m_method; }

    void call(const HTTPRequest& req, HTTPResponse& res) const;

    void set_functor(Method method, EndpointFunctor functor);

    bool is_valid() const { return m_is_valid; }

  private:
    Endpoint()
        : m_is_valid(false) {}

  public:
    static Endpoint null() { return Endpoint(); }

  private:
    bool m_is_valid;

    std::string m_name;
    Method      m_method;

    EndpointFunctor m_functor;
  };

  class Namespace {
  public:
    Namespace(const std::string& name);
    ~Namespace();

    Endpoint match(const std::vector<std::string>& path, std::size_t pos = 0);

    bool       has_subnamespace(const std::string& name);
    Namespace* get_subnamespace(const std::string& name);
    void       add_subnamespace(const std::string& name, Namespace* ns);

    void set_endpoint(Method method, EndpointFunctor functor);

  private:
    std::string m_name;

    std::optional<Endpoint>                     m_endpoint;
    std::unordered_map<const char*, Namespace*> m_subnamespaces;
  };

} // namespace Oxy::NetRender::HTTP
