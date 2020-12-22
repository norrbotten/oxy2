#pragma once

#include <cassert>

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONString final : public JSONValue {
  public:
    virtual bool is_string() const override { return true; }

    virtual std::string stringify() const override { return m_value; }

    auto& value() { return m_value; }

    static bool parse_literal(const std::string& literal, JSONString* res) {
      assert(literal.size() >= 2);
      res->value() = literal.substr(1, literal.size() - 2);
      // TODO: escaped characters
      return true;
    }

  private:
    std::string m_value;
  };

} // namespace Oxy::NetRender::JSON
