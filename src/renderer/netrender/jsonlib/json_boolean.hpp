#pragma once

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONBoolean final : public JSONValue {
  public:
    virtual bool is_boolean() const override { return true; }

    virtual std::string stringify() const override { return m_value ? "true" : "false"; }

    auto& value() { return m_value; }

    static bool parse_literal(const std::string& literal, JSONBoolean* res) {
      if (literal == "true") {
        res->value() = true;
        return false;
      }
      else if (literal == "false") {
        res->value() = false;
        return false;
      }

      return true;
    }

  private:
    bool m_value;
  };

  template <typename Ret>
  struct json_get<JSONType::Boolean, Ret> {
    Ret operator()(const JSONValue* val) {
      JSONBoolean* boolean = (JSONBoolean*)val;
      return boolean->value();
    }
  };

} // namespace Oxy::NetRender::JSON
