#pragma once

#include <iostream>

#include "renderer/common/ast_builder.hpp"
#include "renderer/common/parser.hpp"

#include "renderer/netrender/jsonlib/ast_node.hpp"

namespace Oxy::NetRender::JSON {

  using ASTBuilder = Common::ASTBuilder<ASTNode>;

  class Parser final : public Common::Parser<ASTBuilder> {
  public:
    Parser(const std::string& input)
        : Common::Parser<ASTBuilder>(input) {}

    bool match_number_sign() {
      return match([&] {
        if (ch() == '-')
          forward();

        return true;
      });
    }

    bool match_number_fraction() {
      return match([&] {
        if (ch() == '.') {
          forward();

          if (!Common::is_digit(ch()))
            return false;

          while (Common::is_digit(ch()))
            forward();

          return true;
        }
        else
          return true;
      });
    }

    bool match_number_exponent() {
      return match([&] {
        if (ch() == 'e' || ch() == 'E') {
          forward();

          if (ch() == '-' || ch() == '+')
            forward();

          if (!Common::is_digit(ch()))
            return false;

          forward();

          while (Common::is_digit(ch()))
            forward();

          return true;
        }
        else
          return true;
      });
    }

    bool match_number_pre_fraction() {
      return match([&] {
        if (ch() == '0') {
          forward();
          return true;
        }
        else {
          if (!Common::is_digit_1_to_9(ch()))
            return false;

          forward();

          while (Common::is_digit(ch()))
            forward();

          return true;
        }
      });
    }

    bool match_number() {
      return match([&] {
        discard();

        optional([&] { return match_number_sign(); });

        if (!match_number_pre_fraction())
          return false;

        if (match_number_fraction() && match_number_exponent()) {
#ifndef TEST_JSON_NON_AST
          m_ast.node()->assign_value_type(JSONType::Number);
          m_ast.node()->assign_value_literal(consume());
#endif

          return true;
        }

        return false;
      });
    }

    bool match_4_hex_digits() {
      return match([&] {
        for (int i = 0; i < 4; i++) {
          if (!Common::is_hex_digit(ch()))
            return false;

          forward();
        }

        return true;
      });
    }

    bool match_escape_sequence() {
      return match([&] {
        if (ch() == '\\') {
          forward();

          if (ch() == 'u') {
            forward();
            return match_4_hex_digits();
          }
          else if (Common::is_valid_escape(ch())) {
            forward();
            return true;
          }

          return false;
        }
        else
          return false;
      });
    }

    bool match_noncontrol_char() {
      return match([&] {
        if (Common::is_control_char(ch()) || ch() == '"' || ch() == '\\')
          return false;

        forward();

        return true;
      });
    }

    bool match_string_character() {
      return match([&] { return match_escape_sequence() || match_noncontrol_char(); });
    }

    bool match_string() {
      return match([&] {
        discard();

        if (ch() != '"')
          return false;

        forward();

        while (match_string_character())
          ;

        if (ch() != '"')
          return false;

        forward();

#ifndef TEST_JSON_NON_AST
        m_ast.node()->assign_value_type(JSONType::String);
        m_ast.node()->assign_value_literal(consume());
#endif

        return true;
      });
    }

    bool match_null() {
      return match([&] {
        if (peek(0, 4) == "null") {
          forward(4);

#ifndef TEST_JSON_NON_AST
          m_ast.node()->assign_value_type(JSONType::Null);
#endif

          return true;
        }

        return false;
      });
    }

    bool match_boolean() {
      return match([&] {
        discard();

        if (peek(0, 4) == "true") {
          forward(4);

#ifndef TEST_JSON_NON_AST
          m_ast.node()->assign_value_type(JSONType::Boolean);
          m_ast.node()->assign_value_literal(consume());
#endif

          return true;
        }

        if (peek(0, 5) == "false") {
          forward(5);

#ifndef TEST_JSON_NON_AST
          m_ast.node()->assign_value_type(JSONType::Boolean);
          m_ast.node()->assign_value_literal(consume());
#endif

          return true;
        }

        return false;
      });
    }

    bool match_object_keyvalue_separator() {
      return match([&] {
        if (ch() == ',') {
          forward();
          return true;
        }

        return false;
      });
    }

    bool match_object_string_value_separator() {
      return match([&] {
        if (ch() == ':') {
          forward();
          return true;
        }

        return false;
      });
    }

    bool match_object_string_value() {
      return match([&] {
        skip_whitespace();

#ifndef TEST_JSON_NON_AST
        // we call match_string directly here, so we need to push a new ast node
        m_ast.push(ASTNode::create_value());
#endif

        if (!match_string())
          return false;

        skip_whitespace();

        if (!match_object_string_value_separator())
          return false;

        if (!match_value())
          return false;

        return true;
      });
    }

    bool match_object() {
      return match([&] {
        if (ch() != '{')
          return false;

        forward();

        skip_whitespace();

        if (ch() == '}') {
          forward();
          m_ast.node()->assign_value_type(JSONType::Object);
          return true;
        }

        bool trailing_comma = false;

        while (true) {
          if (match_object_string_value()) {
            trailing_comma = false;

#ifndef TEST_JSON_NON_AST
            m_ast.reverse(2);
            m_ast.bubble(-1); // key, -1 to go past the value thats above on the stack
            m_ast.bubble();   // value
#endif

            if (match_object_keyvalue_separator())
              trailing_comma = true;
          }
          else {
#ifndef TEST_JSON_NON_AST
            // reject the key object
            m_ast.reject();
#endif
            break;
          }
        }

        if (trailing_comma)
          return false;

        skip_whitespace();

        if (ch() != '}')
          return false;

        forward();

        m_ast.node()->assign_value_type(JSONType::Object);

        return true;
      });
    }

    bool match_value() {
      return match([&] {
#ifndef TEST_JSON_NON_AST
        m_ast.push(ASTNode::create_value());
#endif

        skip_whitespace();

        if (!(match_string() || match_number() || match_object() || match_array() ||
              match_boolean() || match_null())) {

#ifndef TEST_JSON_NON_AST
          m_ast.reject();
#endif
          return false;
        }

        skip_whitespace();

        return true;
      });
    }

    bool match_array_separator() {
      return match([&] {
        if (ch() == ',') {
          forward();
          return true;
        }

        return false;
      });
    }

    bool match_array() {
      return match([&] {
        if (ch() != '[')
          return false;

        forward();

        bool trailing_comma = false;

        while (true) {
          if (match_value()) {
            trailing_comma = false;

#ifndef TEST_JSON_NON_AST
            m_ast.bubble();
#endif

            if (match_array_separator())
              trailing_comma = true;
          }
          else
            break;
        }

        if (trailing_comma)
          return false;

        skip_whitespace();

        if (ch() != ']')
          return false;

        forward();

        m_ast.node()->assign_value_type(JSONType::Array);

        return true;
      });
    }

    bool parse() { return match_value(); }

    auto ast() { return m_ast.ast(); }

  private:
    std::string m_input;
    int         m_position, m_consume_ptr;

    ASTBuilder m_ast;
  };

} // namespace Oxy::NetRender::JSON
