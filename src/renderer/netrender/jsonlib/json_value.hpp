#pragma once

#include <iostream>

namespace Oxy::NetRender::JSON {

  class JSONValue {
  public:
    virtual ~JSONValue() {}

    virtual bool is_object() const { return false; }
    virtual bool is_array() const { return false; }
    virtual bool is_string() const { return false; }
    virtual bool is_number() const { return false; }
    virtual bool is_boolean() const { return false; }
    virtual bool is_null() const { return false; }

    virtual std::string stringify() const = 0;
  };

} // namespace Oxy::NetRender::JSON
