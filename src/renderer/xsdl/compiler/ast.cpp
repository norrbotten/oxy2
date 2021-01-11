#include "renderer/xsdl/compiler/ast.hpp"

#include <iostream>

namespace Oxy::XSDL::Compiler {

  bool ASTBuilder::build_number_literal_expression() {
    return match([&] {
      if (auto t = matches(TokenType::NumberConstant); t.valid()) {
        forward();
        push(new AST::NumberConstantExpressionNode(t.token()));
        return true;
      }

      return false;
    });
  }

  bool ASTBuilder::build_string_literal_expression() {
    return match([&] {
      if (auto t = matches(TokenType::StringConstant); t.valid()) {
        forward();
        push(new AST::StringConstantExpressionNode(t.token()));
        return true;
      }

      return false;
    });
  }

  bool ASTBuilder::build_identifier_expression() {
    return match([&] {
      if (auto t = matches(TokenType::StringConstant); t.valid()) {
        forward();
        push(new AST::IdentifierExpressionNode(t.token()));
        return true;
      }

      return false;
    });
  }

  bool ASTBuilder::build_parenthesis_expression() {
    push(new AST::ParenthesisExpressionNode());

    if (match([&] {
          if (auto t = matches(TokenType::Parenthesis, "("); t.valid())
            forward();
          else
            return false;

          if (!build_expression())
            return false;

          if (auto t = matches(TokenType::Parenthesis, ")"); t.valid())
            forward();
          else
            return false;

          // (expr)
          auto expr = (AST::ExpressionNode*)reduce();

          auto paren_expr = (AST::ParenthesisExpressionNode*)top();

          paren_expr->expression() = expr;

          return true;
        })) {
      return true;
    }
    else {
      pop();
      return false;
    }
  }

  bool ASTBuilder::build_indexing_expression() {
    return false;

    if (end_of_tokens())
      return false;

    push(new AST::IndexingExpressionNode());

    if (match([&] {
          if (!build_expression()) {
            return false;
          }

          if (auto t = matches(TokenType::Bracket, "["); t.valid())
            forward();
          else
            return false;

          if (!build_expression())
            return false;

          if (auto t = matches(TokenType::Bracket, "]"); t.valid())
            forward();
          else
            return false;

          return true;
        })) {

      // expr[index_expr]
      auto index_expr = (AST::ExpressionNode*)reduce();
      auto expr       = (AST::ExpressionNode*)reduce();

      auto indexing_expr = (AST::IndexingExpressionNode*)top();

      indexing_expr->expression()       = expr;
      indexing_expr->index_expression() = index_expr;

      return true;
    }
    else {
      pop(); // pop IndexingExpressionNode
      return false;
    }
  }

  bool ASTBuilder::build_expression_non_leftrec_cases() {
    return match([&] {
      if (build_number_literal_expression())
        return true;
      else if (build_string_literal_expression())
        return true;
      else if (build_identifier_expression())
        return true;
      else if (build_parenthesis_expression())
        return true;
      else
        return false;
    });
  }

  bool ASTBuilder::build_expression() {
    return match([&] {
      if (build_indexing_expression())
        return true;
      else
        return false;
    });
  }

  bool ASTBuilder::build_compound_statement() {
    if (end_of_tokens())
      return false;

    auto stmt = new AST::CompoundStatementNode();
    push(stmt);

    if (match([&] {
          if (auto t = matches(TokenType::SquigglyBracket, "{"); t.valid()) {
            forward();
            return false;
          }

          while (true) {
            if (build_statement())
              stmt->statements().push_back((AST::StatementNode*)reduce());
            else
              break;
          }

          if (auto t = matches(TokenType::SquigglyBracket, "}"); t.valid()) {
            forward();
            return false;
          }

          return true;
        })) {
      return true;
    }
    else {
      pop(); // pop new CompoundStatementNode
      return false;
    }
  }

  bool ASTBuilder::build_expression_statement() {
    return match([&] {
      if (build_expression()) {
        if (auto t = matches(TokenType::EndOfStatement); !t.valid()) {
          pop();
          return false;
        }

        return true;
      }

      return false;
    });
  }

  bool ASTBuilder::build_no_op_statement() {
    return match([&] {
      if (auto t = matches(TokenType::EndOfStatement); t.valid()) {
        forward();
        push(new AST::NoOpStatementNode());
        return true;
      }

      return false;
    });
  }

  bool ASTBuilder::build_statement() {
    return match([&] {
      if (build_no_op_statement())
        return true;
      else if (build_expression_statement())
        return true;
      else if (build_compound_statement())
        return true;
      else
        return false;
    });
  }

} // namespace Oxy::XSDL::Compiler