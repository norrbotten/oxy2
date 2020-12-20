#pragma once

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONNull final : public JSONValue {
  public:
    virtual bool is_null() const override { return true; }

    virtual std::string stringify() const override { return "null"; }
  };

} // namespace Oxy::NetRender::JSON
