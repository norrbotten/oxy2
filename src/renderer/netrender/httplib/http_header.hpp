#pragma once

#include <algorithm>
#include <string>
#include <utility>

#include <string.h>

namespace Oxy::NetRender::HTTP {

  auto canonicalize = [](const std::string& str) -> std::string {
    auto cpy = std::string(str);
    std::transform(cpy.begin(), cpy.end(), cpy.begin(), [](auto ch) { return std::tolower(ch); });
    return cpy;
  };

  class HTTPHeader {
  public:
    HTTPHeader();
    HTTPHeader(const std::string& name, const std::string& content);

    const auto& name() const { return m_name; }
    const auto& content() const { return m_content; }
    const auto  valid() const { return m_is_valid; }

    bool compare(const char* against) const {
      return m_is_valid && m_content.compare(against) == 0;
    }

    bool compare(const std::string& against) const { return m_is_valid && against == m_content; }

    template <typename T>
    bool operator==(T& cmp_against) {
      if (!m_is_valid)
        return false;

      return std::string(cmp_against) == m_content;
    }

  private:
    bool m_is_valid;

    std::string m_name;
    std::string m_content;
  };

} // namespace Oxy::NetRender::HTTP
