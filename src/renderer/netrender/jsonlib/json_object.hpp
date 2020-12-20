#pragma once

#include <sstream>
#include <unordered_map>

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  class JSONArray final : public JSONValue {
  public:
    virtual bool is_object() const override { return true; }

    virtual std::string stringify() const override {
      std::stringstream ss;

      ss << "{ ";

      for (auto it = m_hashmap.begin(); it != m_hashmap.end();) {
        auto& [key, value] = *it;
        ss << "\"" << key << "\": " << value->stringify();

        auto it_cpy = it;
        it_cpy++;
        if (it_cpy != m_hashmap.end())
          ss << ", ";

        it++;
      }

      ss << "} ";

      return ss.str();
    }

    auto& hashmap() { return m_hashmap; }

  private:
    std::unordered_map<std::string, JSONValue*> m_hashmap;
  };

} // namespace Oxy::NetRender::JSON
