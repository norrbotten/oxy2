#pragma once

#include <sstream>

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONNumber final : public JSONValue {
  public:
    virtual bool is_number() const override { return true; }

    virtual std::string stringify() const override { return std::to_string(m_value); }

    auto& value() { return m_value; }

    static bool parse_literal(const std::string& literal, JSONNumber* res) {
      std::istringstream iss(literal);
      iss >> res->value();
      return !iss.fail();
    }

  private:
    double m_value;
  };

} // namespace Oxy::NetRender::JSON
