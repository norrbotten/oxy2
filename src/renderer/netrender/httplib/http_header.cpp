#include "renderer/netrender/httplib/http_header.hpp"

namespace Oxy::NetRender::HTTP {

  HTTPHeader::HTTPHeader()
      : m_is_valid(false) {}

  HTTPHeader::HTTPHeader(const std::string& name, const std::string& content)
      : m_is_valid(true)
      , m_name(canonicalize(name))
      , m_content(content) {}

} // namespace Oxy::NetRender::HTTP
