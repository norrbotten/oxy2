#pragma once

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONBoolean final : public JSONValue {
  public:
    virtual bool is_boolean() const override { return true; }

    virtual std::string stringify() const override { return m_value ? "true" : "false"; }

    auto& value() { return m_value; }

  private:
    bool m_value;
  };

} // namespace Oxy::NetRender::JSON