#pragma once

#include <sstream>
#include <vector>

#include "renderer/netrender/jsonlib/json_value.hpp"

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

    auto& values() { return m_values; }

  private:
    std::vector<JSONValue*> m_values;
  };

} // namespace Oxy::NetRender::JSON
