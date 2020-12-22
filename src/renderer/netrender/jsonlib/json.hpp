#pragma once

#include <string>

#include "renderer/netrender/jsonlib/parser.hpp"

namespace Oxy::NetRender::JSON {

  class JSONParser {
  public:
    JSONParser(const std::string& data)
        : m_parser(data) {

      if (m_parser.parse()) {
        m_value = m_parser.ast()->to_json_value();
      }
    }

    ~JSONParser() {
      if (m_value != nullptr)
        delete m_value;
    }

    bool success() const { return m_value != nullptr; }
    bool failed() const { return m_value == nullptr; }

    auto json() const { return m_value; }

  private:
    Parser     m_parser;
    JSONValue* m_value;
  };

} // namespace Oxy::NetRender::JSON
