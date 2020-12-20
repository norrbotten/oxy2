#pragma once

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONNumber final : public JSONValue {
  public:
    virtual bool is_number() const override { return true; }

    virtual std::string stringify() const override { return std::to_string(m_value); }

    auto& value() { return m_value; }

  private:
    double m_value;
  };

} // namespace Oxy::NetRender::JSON
