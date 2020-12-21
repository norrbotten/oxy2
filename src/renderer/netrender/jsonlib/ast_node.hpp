#pragma once

#include <string>
#include <vector>

#include "renderer/netrender/jsonlib/json_value.hpp"

namespace Oxy::NetRender::JSON {

  enum class JSONType {
    NUMBER,
    STRING,
    ARRAY,
    OBJECT,
    BOOLEAN,
    NOLL, // fucking NULL macro
  };

  using JSONLiteral = std::string;

  class ASTNode {
    ASTNode(JSONType type, JSONLiteral literal)
        : m_type(type)
        , m_literal(literal) {}

  public:
    static ASTNode* create_number(const JSONLiteral& literal) {
      return new ASTNode(JSONType::NUMBER, literal);
    }

    static ASTNode* create_string(const JSONLiteral& literal) {
      return new ASTNode(JSONType::STRING, literal);
    }

    static ASTNode* create_array() { return new ASTNode(JSONType::ARRAY, ""); }

    static ASTNode* create_object() { return new ASTNode(JSONType::OBJECT, ""); }

    static ASTNode* create_boolean(const JSONLiteral& literal) {
      return new ASTNode(JSONType::BOOLEAN, literal);
    }

    static ASTNode* create_null() { return new ASTNode(JSONType::NOLL, ""); }

    ~ASTNode() {
      for (auto& child : m_children)
        delete child;
    }

    bool has_children() const { return m_children.size() > 0; }

    const auto& children() const { return m_children; }
    const auto& type() const { return m_type; }
    const auto& literal() const { return m_literal; }

    void push(ASTNode* node) { m_children.push_back(node); }

  private:
    std::vector<ASTNode*> m_children;

    JSONType    m_type;
    JSONLiteral m_literal;
  };

} // namespace Oxy::NetRender::JSON
