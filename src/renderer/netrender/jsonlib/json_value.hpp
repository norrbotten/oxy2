#pragma once

#include <iostream>

namespace Oxy::NetRender::JSON {

  enum class JSONType {
    Value, // only used and is valid briefly in the parsing stage
    Number,
    String,
    Array,
    Object,
    Boolean,
    Null,
  };

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

  template <JSONType Type, typename Ret>
  struct json_get {
    Ret operator()(const JSONValue* val) = delete;
  };

} // namespace Oxy::NetRender::JSON
