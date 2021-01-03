#pragma once

#include <iostream>
#include <vector>

#include "renderer/xsdl/compiler/ast_node.hpp"
#include "renderer/xsdl/compiler/token.hpp"

namespace Oxy::XSDL::Compiler {

  class TokenProxy {
  public:
    TokenProxy() = default;
    TokenProxy(const Token& token)
        : m_token(token)
        , m_token_valid(true) {}

    auto& token() const { return m_token; }
    bool  valid() const { return m_token_valid; }

    operator bool() { return m_token_valid; }

  private:
    Token m_token;
    bool  m_token_valid{false};
  };

  // ASTBuilder has a similar pattern to the tokenizer
  class ASTBuilder {
  public:
    ASTBuilder(const std::vector<Token>& tokens)
        : m_tokens_ref(tokens) {}

  private:
    template <typename MatchFunctor>
    bool match(MatchFunctor functor) {
      int pos = m_position;
      int con = m_consume;

      if (!functor()) {
        m_position = pos;
        m_consume  = con;
        return false;
      }

      return true;
    }

    TokenProxy matches(TokenType type, const std::string& literal = "", int offset = 0) {
      if ((m_position + offset) >= m_tokens_ref.size())
        return TokenProxy();

      auto& cur = m_tokens_ref.at(m_position + offset);
      return (cur.type == type && (true || (literal.size() > 0 && cur.literal == literal)))
                 ? TokenProxy(cur)
                 : TokenProxy();
    }

    void forward() { m_position++; }

    void push(AST::Node* node) { m_stack.push_back(node); }
    void pop() {
      auto node = m_stack.back();
      m_stack.pop_back();
      delete node;
    }

    AST::Node* top() const { return m_stack.back(); }

    AST::Node* reduce() {
      auto node = m_stack.back();
      m_stack.pop_back();
      return node;
    }

    bool end_of_tokens() const { return m_position >= m_tokens_ref.size(); }

    bool build_number_literal_expression();
    bool build_string_literal_expression();
    bool build_identifier_expression();

    bool build_parenthesis_expression();
    bool build_indexing_expression();

    bool build_expression_non_leftrec_cases();
    bool build_expression();

    bool build_compound_statement();
    bool build_conditional_statement();
    bool build_while_statement();
    bool build_for_statement();
    bool build_return_statement();
    bool build_expression_statement();
    bool build_no_op_statement();

    bool build_statement();

  public:
    bool process() {
      push(new AST::CompoundStatementNode());
      if (auto ret = build_statement(); ret) {
        auto stmt = (AST::StatementNode*)reduce();

        auto cmpstmt = (AST::CompoundStatementNode*)top();
        cmpstmt->statements().push_back(stmt);

        return true;
      }

      return false;
    }

    auto ast() const { return top(); }

  private:
    const std::vector<Token>& m_tokens_ref;
    int                       m_position{0}, m_consume{0};

    std::vector<AST::Node*> m_stack;
  };

} // namespace Oxy::XSDL::Compiler
