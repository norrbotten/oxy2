#pragma once

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONNumber final : public JSONValue {
  public:
    virtual bool is_string() const override { return true; }

    virtual std::string stringify() const override { return m_value; }

    auto& value() { return m_value; }

  private:
    std::string m_value;
  };

} // namespace Oxy::NetRender::JSON
