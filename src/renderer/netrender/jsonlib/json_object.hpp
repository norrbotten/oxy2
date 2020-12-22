#pragma once

#include <sstream>
#include <unordered_map>

#include "renderer/netrender/jsonlib/json_string.hpp"

namespace Oxy::NetRender::JSON {

  class JSONObject final : public JSONValue {
  public:
    virtual ~JSONObject() {
      for (auto& [key, value] : m_hashmap) {
        delete key;
        delete value;
      }
    }

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

    void append(JSONString* key, JSONValue* value) { m_hashmap[key] = value; }

    auto& hashmap() { return m_hashmap; }

  private:
    std::unordered_map<JSONString*, JSONValue*> m_hashmap;
  };

} // namespace Oxy::NetRender::JSON
