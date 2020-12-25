#include "renderer/xsdl/compiler/parser.hpp"

#include <array>
#include <cstring>
#include <iostream>

namespace Oxy::XSDL::Compiler {

  // Parses an identifier, basically a string of valid alphanumeric characters including _$ but not
  // starting with a number
  Token Parser::parse_identifier() {
    if (match([&] {
          if (!Common::is_valid_first_identifier_char(ch()))
            return false;

          discard();

          while (Common::is_valid_identifier_char(ch()))
            forward();

          return true;
        })) {
      return consume();
    }

    return {};
  }

  // Parses an integer literal
  Token Parser::parse_integer_literal() {
    if (match([&] {
          discard();

          if (ch() == '-')
            forward();

          if (!Common::is_digit(ch()))
            return false;

          while (Common::is_digit(ch()))
            forward();

          return true;
        })) {
      return consume();
    }

    return {};
  }

  // Parses a decimal number literal
  Token Parser::parse_decimal_literal() {
    if (match([&] {
          discard();

          // sign
          if (ch() == '-')
            forward();

          // digits before .
          while (Common::is_digit(ch()))
            forward();

          if (ch() == '.') {
            forward();

            // need at least 1 digit after .
            if (!Common::is_digit(ch()))
              return false;

            while (Common::is_digit(ch()))
              forward();
          }
          else
            return false; // decimals need to have a dot

          // might have an exponent
          if (ch() == 'e' || ch() == 'E') {
            forward();

            // exponent sign
            if (ch() == '-' || ch() == '+')
              forward();

            // need a digit after exponent
            if (!Common::is_digit(ch()))
              return false;

            while (Common::is_digit(ch()))
              forward();
          }

          return true;
        })) {
      return consume();
    }

    return {};
  }

  // Parses any keyword
  Token Parser::parse_keyword() {
    static auto keywords = std::array{
        "if", "else", "else if", "for", "while", "return",
    };

    if (match([&] {
          for (auto& word : keywords) {
            auto len = strlen(word);
            if (peek(0, len) == word) {
              forward(len);
              return true;
            }
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  // Parses any operator
  Token Parser::parse_operator() {
    /* clang-format off */
    // All operators, these need to be in ordered longest first
    static auto operators = std::array{
      "&&",
      "||",
      "+=",
      "-=",
      "*=",
      "/=",
      "==",
      "!=",
      "++",
      "--",
      "+",
      "-",
      "*",
      "/",
      ",",
      "=",
    };
    /* clang-format on */

    if (match([&] {
          for (auto& op : operators) {
            auto len = strlen(op);
            if (peek(0, len) == op) {
              forward(len);
              return true;
            }
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  Token Parser::parse_left_parenthesis() {
    if (match([&] {
          if (ch() == '(') {
            forward();
            return true;
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  Token Parser::parse_right_parenthesis() {
    if (match([&] {
          if (ch() == ')') {
            forward();
            return true;
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  Token Parser::parse_left_bracket() {
    if (match([&] {
          if (ch() == '[') {
            forward();
            return true;
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  Token Parser::parse_right_bracket() {
    if (match([&] {
          if (ch() == ']') {
            forward();
            return true;
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  Token Parser::parse_left_sqbracket() {
    if (match([&] {
          if (ch() == '{') {
            forward();
            return true;
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  Token Parser::parse_right_sqbracket() {
    if (match([&] {
          if (ch() == '}') {
            forward();
            return true;
          }

          return false;
        })) {
      return consume();
    }

    return {};
  }

  bool Parser::match_identifier() {
    return match([&] {
      if (auto ident = parse_identifier(); ident.has_value()) {
        m_ast.push(new IdentifierNode(ident.value()));
        m_ast.bubble();
        return true;
      }

      return false;
    });
  }

  bool Parser::match_integer_literal() {
    return match([&] {
      if (auto ident = parse_integer_literal(); ident.has_value()) {
        m_ast.push(new IntegerLiteralNode(ident.value()));
        m_ast.bubble();
        return true;
      }

      return false;
    });
  }

  bool Parser::match_decimal_literal() {
    return match([&] {
      if (auto ident = parse_decimal_literal(); ident.has_value()) {
        m_ast.push(new DecimalLiteralNode(ident.value()));
        m_ast.bubble();
        return true;
      }

      return false;
    });
  }

  bool Parser::match_left_parenthesis() {
    return match([&] {
      if (auto ident = parse_left_parenthesis(); ident.has_value()) {
        m_ast.push(new ParenthesisNode(ParenthesisType::Left));
        m_ast.bubble();
        return true;
      }

      return false;
    });
  }

  bool Parser::match_right_parenthesis() {
    return match([&] {
      if (auto ident = parse_right_parenthesis(); ident.has_value()) {
        m_ast.push(new ParenthesisNode(ParenthesisType::Right));
        m_ast.bubble();
        return true;
      }

      return false;
    });
  }

  bool Parser::match_literal_expression() {
    return match([&] {
      if (match_identifier()) {
        return true;
      }
      else if (match_decimal_literal()) {
        return true;
      }
      else if (match_integer_literal()) {
        return true;
      }
      else if (match_left_parenthesis() && match_expression() && match_right_parenthesis()) {
        m_ast.bubble();
        return true;
      }
      else
        return false;
    });
  }

  bool Parser::match_expression() {
    m_ast.push(new ExpressionStatementNode());
    if (match([&] {
          if (match_literal_expression()) {
            return true;
          }
          else {
            return false;
          }
        })) {
      return true;
    }
    else {
      m_ast.reject();
      return false;
    }
  }

} // namespace Oxy::XSDL::Compiler
