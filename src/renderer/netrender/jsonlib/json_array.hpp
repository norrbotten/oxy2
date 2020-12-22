#pragma once

#include <sstream>
#include <type_traits>
#include <vector>

#include "renderer/netrender/jsonlib/json_value.hpp"

#include "renderer/netrender/jsonlib/json_boolean.hpp"
#include "renderer/netrender/jsonlib/json_number.hpp"
#include "renderer/netrender/jsonlib/json_string.hpp"

namespace Oxy::NetRender::JSON {

  class JSONArray final : public JSONValue {
  public:
    virtual ~JSONArray() {
      for (auto& value : m_values)
        delete value;
    }

    virtual bool is_array() const override { return true; }

    virtual std::string stringify() const override {
      std::stringstream ss;
      ss << "[";

      for (auto it = m_values.begin(); it != m_values.end();) {
        ss << (*it)->stringify();

        if ((it + 1) != m_values.end())
          ss << ", ";

        it++;
      }

      return ss.str();
    }

    void append(JSONValue* value) { m_values.push_back(value); }

    auto& values() { return m_values; }

    template <typename T>
    std::vector<T> to_vector() const {
      std::vector<T> res;

      static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> ||
                    std::is_same_v<T, std::string>);

      for (auto& value : m_values) {
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
          if (value->is_number()) {
            auto val = json_get<JSONType::Number, T>{}((JSONNumber*)value);
            res.push_back(val);
          }
          else if (value->is_boolean()) {
            auto val = json_get<JSONType::Boolean, T>{}((JSONBoolean*)value);
            res.push_back(val);
          }
          else {
            res.push_back(T());
          }
        }
        else if constexpr (std::is_same_v<T, std::string>) {
          if (value->is_string()) {
            auto val = json_get<JSONType::String, T>{}((JSONString*)value);
            res.push_back(val);
          }
          else {
            res.push_back(T());
          }
        }
      }

      return res;
    }

  private:
    std::vector<JSONValue*> m_values;
  };

} // namespace Oxy::NetRender::JSON
