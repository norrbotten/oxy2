#pragma once

#include <string>
#include <vector>

#include "renderer/macros.hpp"

#include "renderer/xsdl/compiler/token.hpp"

namespace Oxy::XSDL::Compiler::AST { // sticking these in an extra namespace

  class Node {
  public:
    virtual ~Node() = default;

    virtual std::string class_name() const { return "Node"; }
  };

  //
  // EXPRESSION NODES
  //

  class ExpressionNode : public Node {
  public:
    virtual std::string class_name() const override { return "ExpressionNode"; }
  };

  //

  // NumberConstantToken
  class NumberConstantExpressionNode final : public ExpressionNode {
  public:
    NumberConstantExpressionNode(const Token& token)
        : m_token(token) {}

    virtual std::string class_name() const override { return "NumberExpressionNode"; }

    REF(token); // dont need the semicolon, but the syntax highlighter is funny without it

  private:
    Token m_token;
  };

  //

  // StringConstantToken
  class StringConstantExpressionNode final : public ExpressionNode {
  public:
    StringConstantExpressionNode(const Token& token)
        : m_token(token) {}

    virtual std::string class_name() const override { return "StringConstantExpressionNode"; }

    REF(token);

  private:
    Token m_token;
  };

  //

  // IdentifierToken
  class IdentifierExpressionNode final : public ExpressionNode {
  public:
    IdentifierExpressionNode(const Token& token)
        : m_token(token) {}

    virtual std::string class_name() const override { return "IdentifierExpressionNode"; }

    REF(token);

  private:
    Token m_token;
  };

  //

  // '(' ExpressionNode ')'
  class ParenthesisExpressionNode final : public ExpressionNode {
  public:
    virtual ~ParenthesisExpressionNode() {
      if (m_expression != nullptr)
        delete m_expression;
    }

    virtual std::string class_name() const override { return "ParenthesisExpressionNode"; }

    REF(expression);

  private:
    ExpressionNode* m_expression{nullptr};
  };

  //

  // ExpressionNode '[' ExpressionNode ']'
  class IndexingExpressionNode final : public ExpressionNode {
  public:
    virtual ~IndexingExpressionNode() {
      if (m_expression != nullptr)
        delete m_expression;

      if (m_index_expression != nullptr)
        delete m_index_expression;
    }
    virtual std::string class_name() const override { return "IndexingExpressionNode"; }

    REF(expression);
    REF(index_expression);

  private:
    ExpressionNode* m_expression{nullptr};
    ExpressionNode* m_index_expression{nullptr};
  };

  //
  // STATEMENT NODES
  //

  class StatementNode : public Node {
  public:
    virtual std::string class_name() const override { return "StatementNode"; }
  };

  //

  // StatementNode ';' ... ';'
  class CompoundStatementNode final : public StatementNode {
  public:
    virtual ~CompoundStatementNode() {
      for (auto& statement : m_statements)
        delete statement;
    }

    virtual std::string class_name() const override { return "CompoundStatementNode"; }

    REF(statements);

  private:
    std::vector<StatementNode*> m_statements;
  };

  //

  // 'if' ExpressionNode '{' StatementNode '}'
  class ConditionalStatementNode final : public StatementNode {
  public:
    virtual ~ConditionalStatementNode() {
      if (m_condition != nullptr)
        delete m_condition;

      if (m_body != nullptr)
        delete m_body;
    }

    virtual std::string class_name() const override { return "ConditionalStatementNode"; }

    REF(condition);
    REF(body);

  private:
    ExpressionNode* m_condition{nullptr};
    StatementNode*  m_body{nullptr};
  };

  //

  // 'while' ExpressionNode '{' StatementNode '}'
  class WhileStatementNode final : public StatementNode {
  public:
    virtual ~WhileStatementNode() {
      if (m_condition != nullptr)
        delete m_condition;

      if (m_body != nullptr)
        delete m_body;
    }

    virtual std::string class_name() const override { return "WhileStatementNode"; }

    REF(condition);
    REF(body);

  private:
    ExpressionNode* m_condition{nullptr};
    StatementNode*  m_body{nullptr};
  };

  //

  // 'for' ExpressionNode ';' ExpressionNode ';' 'ExpressionNode' '{' StatementNode '}'
  class ForStatementNode final : public StatementNode {
  public:
    virtual ~ForStatementNode() {
      if (m_pre != nullptr)
        delete m_pre;

      if (m_condition != nullptr)
        delete m_condition;

      if (m_post != nullptr)
        delete m_post;

      if (m_body != nullptr)
        delete m_body;
    }

    virtual std::string class_name() const override { return "ForStatementNode"; }

    REF(pre);
    REF(condition);
    REF(post);
    REF(body);

  private:
    ExpressionNode* m_pre{nullptr};
    ExpressionNode* m_condition{nullptr};
    ExpressionNode* m_post{nullptr};
    StatementNode*  m_body{nullptr};
  };

  //

  // 'return' ExpressionNode ';'
  class ReturnStatementNode final : public StatementNode {
  public:
    virtual ~ReturnStatementNode() {
      if (m_expression != nullptr)
        delete m_expression;
    }

    virtual std::string class_name() const override { return "ReturnStatementNode"; }

    REF(expression);

  private:
    ExpressionNode* m_expression{nullptr};
  };

  //

  // to be decided
  class VariableDefStatementNode final : public StatementNode {
  public:
    virtual ~VariableDefStatementNode() {
      if (m_identifier != nullptr)
        delete m_identifier;

      if (m_expression != nullptr)
        delete m_expression;
    }

    virtual std::string class_name() const override { return "VariableDefStatementNode"; }

    REF(identifier);
    REF(expression);

  private:
    IdentifierExpressionNode* m_identifier{nullptr};
    ExpressionNode*           m_expression{nullptr};
  };

  //

  // ExpressionNode ';'

  class ExpressionStatement final : public StatementNode {
  public:
    virtual std::string class_name() const override { return "ExpressionStatement"; }

    REF(expression);

  private:
    ExpressionNode* m_expression{nullptr};
  };

  //

  // ';'
  class NoOpStatementNode final : public StatementNode {
  public:
    virtual std::string class_name() const override { return "NoOpStatementNode"; }
  };

} // namespace Oxy::XSDL::Compiler::AST
