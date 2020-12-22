#pragma once

#include <cassert>
#include <sstream>
#include <string>
#include <vector>

#include "renderer/netrender/jsonlib/json_value.hpp"

#include "renderer/netrender/jsonlib/json_array.hpp"
#include "renderer/netrender/jsonlib/json_boolean.hpp"
#include "renderer/netrender/jsonlib/json_null.hpp"
#include "renderer/netrender/jsonlib/json_number.hpp"
#include "renderer/netrender/jsonlib/json_object.hpp"
#include "renderer/netrender/jsonlib/json_string.hpp"

namespace Oxy::NetRender::JSON {

  using JSONLiteral = std::string;

  class ASTNode {
    ASTNode(JSONType type, JSONLiteral literal)
        : m_type(type)
        , m_literal(literal) {}

  public:
    ~ASTNode() {
      for (auto& child : m_children)
        delete child;
    }

    static ASTNode* create_value() { return new ASTNode(JSONType::Value, ""); }

    static ASTNode* create_number(const JSONLiteral& literal) {
      return new ASTNode(JSONType::Number, literal);
    }

    static ASTNode* create_string(const JSONLiteral& literal) {
      return new ASTNode(JSONType::String, literal);
    }

    static ASTNode* create_array() { return new ASTNode(JSONType::Array, ""); }

    static ASTNode* create_object() { return new ASTNode(JSONType::Object, ""); }

    static ASTNode* create_boolean(const JSONLiteral& literal) {
      return new ASTNode(JSONType::Boolean, literal);
    }

    static ASTNode* create_null() { return new ASTNode(JSONType::Null, ""); }

  public:
    std::string stringify() {
      std::stringstream ss;

      switch (m_type) {
      case JSONType::Value: ss << "$VALUE$"; break;
      case JSONType::Number:
      case JSONType::String:
      case JSONType::Boolean: ss << m_literal; break;
      case JSONType::Null: ss << "null"; break;
      case JSONType::Array: {
        ss << "[";
        for (auto it = m_children.begin(); it != m_children.end();) {
          ss << (*it)->stringify();

          if ((it + 1) != m_children.end())
            ss << ", ";

          it++;
        }
        ss << "]";
      } break;

      case JSONType::Object: {
        ss << "{";
        for (auto it = m_children.begin(); it != m_children.end();) {
          ss << (*it)->stringify();

          it++;

          ss << ": " << (*it)->stringify();

          if ((it + 1) != m_children.end())
            ss << ", ";

          it++;
        }
        ss << "}";
      } break;
      }

      return ss.str();
    }

    std::string dump(int indent = 0) {
      auto getindent = [](int n) {
        std::stringstream ss;
        for (int i = 0; i < n; i++)
          ss << "  ";
        return ss.str();
      };

      std::stringstream ss;

      ss << getindent(indent);

      switch (m_type) {
      case JSONType::Value: ss << "$VALUE$"; break;
      case JSONType::Number: ss << "Number [" << m_literal << "]"; break;
      case JSONType::String: ss << "String [" << m_literal << "]"; break;
      case JSONType::Boolean: ss << "Boolean [" << m_literal << "]"; break;
      case JSONType::Null: ss << "Null"; break;
      case JSONType::Array: ss << "Array"; break;
      case JSONType::Object: ss << "Object"; break;
      }

      for (auto& child : m_children) {
        ss << "\n" << child->dump(indent + 1);
      }

      return ss.str();
    }

    JSONValue* to_json_value() const {
      switch (m_type) {
      case JSONType::Value: assert(false); break;
      case JSONType::Number: {
        auto val = new JSONNumber();
        assert(val->parse_literal(m_literal, val));
        return val;
      } break;

      case JSONType::String: {
        auto val = new JSONString();
        assert(val->parse_literal(m_literal, val));
        return val;
      } break;

      case JSONType::Boolean: {
        auto val = new JSONBoolean();
        assert(val->parse_literal(m_literal, val));
        return val;
      } break;

      case JSONType::Null: return new JSONNull(); break;

      case JSONType::Array: {
        auto val = new JSONArray();

        for (auto& child : m_children)
          val->append(child->to_json_value());

        return val;
      } break;

      case JSONType::Object: {
        auto val = new JSONObject();

        for (auto it = m_children.begin(); it != m_children.end();) {
          auto key = new JSONString();
          assert(key->parse_literal((*it)->m_literal, key));
          it++;

          auto value = (*it)->to_json_value();
          it++;

          val->append(key, value);
        }

        return val;
      } break;
      }

      return nullptr;
    }

    bool has_children() const { return m_children.size() > 0; }

    const auto& children() const { return m_children; }
    const auto& type() const { return m_type; }
    const auto& literal() const { return m_literal; }

    bool is_value() const { return m_type == JSONType::Value; }

    void assign_value_type(JSONType type) {
      assert(m_type == JSONType::Value);
      m_type = type;
    }

    void assign_value_literal(const JSONLiteral& literal) {
      assert(m_type == JSONType::Number || m_type == JSONType::Boolean ||
             m_type == JSONType::String);

      m_literal = literal;
    }

    void push(ASTNode* node) { m_children.push_back(node); }

  private:
    std::vector<ASTNode*> m_children;

    JSONType    m_type;
    JSONLiteral m_literal;
  };

} // namespace Oxy::NetRender::JSON
