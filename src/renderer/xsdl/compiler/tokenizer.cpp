#include "renderer/xsdl/compiler/tokenizer.hpp"

#include <cstring>

namespace Oxy::XSDL::Compiler {

  void Tokenizer::process() {
    m_tokens.clear();

    while (!eof()) {
      skip_whitespace();
      m_streamer.discard();

      if (parse_end_of_statement())
        push_token(TokenType::EndOfStatement);
      else if (parse_boolean())
        push_token(TokenType::BooleanConstant);
      else if (parse_number())
        push_token(TokenType::NumberConstant);
      else if (parse_string())
        push_token(TokenType::StringConstant);
      else if (parse_operator())
        push_token(TokenType::Operator);
      else if (parse_while())
        push_token(TokenType::While);
      else if (parse_if())
        push_token(TokenType::If);
      else if (parse_elseif())
        push_token(TokenType::ElseIf);
      else if (parse_else())
        push_token(TokenType::Else);
      else
        throw TokenizationError("Unknown syntax");
    }
  }

  bool Tokenizer::parse_end_of_statement() {
    return match([&] {
      if (ch() == ';') {
        forward();
        return true;
      }

      return false;
    });
  }

  bool Tokenizer::parse_boolean() {
    return match([&] {
      if (peek(0, 4) == "true") {
        forward(4);
        return true;
      }
      else if (peek(0, 5) == "false") {
        forward(5);
        return true;
      }

      return false;
    });
  }

  bool Tokenizer::parse_number() {
    auto parse_number_sign = [this] {
      return match([&] {
        if (ch() == '-')
          forward();

        return true;
      });
    };

    auto parse_number_fraction = [this] {
      return match([&] {
        if (ch() == '.') {
          forward();

          if (!is_digit(ch()))
            return false;

          while (is_digit(ch()))
            forward();

          return true;
        }
        else
          return true;
      });
    };

    auto parse_number_exponent = [this] {
      return match([&] {
        if (ch() == 'e' || ch() == 'E') {
          forward();

          if (ch() == '-' || ch() == '+')
            forward();

          if (!is_digit(ch()))
            return false;

          forward();

          while (is_digit(ch()))
            forward();

          return true;
        }
        else
          return true;
      });
    };

    auto parse_number_pre_fraction = [this] {
      return match([&] {
        if (ch() == '0') {
          forward();
          return true;
        }
        else {
          if (!is_digit_1_to_9(ch()))
            return false;

          forward();

          while (is_digit(ch()))
            forward();

          return true;
        }
      });
    };

    return match([&] {
      discard();

      parse_number_sign();

      if (!parse_number_pre_fraction())
        return false;

      if (parse_number_fraction() && parse_number_exponent())
        return true;

      return false;
    });
  }

  bool Tokenizer::parse_string() {
    auto parse_escape_sequence = [this] {
      return match([&] {
        if (ch() == '\\') {
          forward();

          if (is_valid_escape(ch())) {
            forward();
            return true;
          }
        }

        return false;
      });
    };

    auto parse_noncontrol_char = [this] {
      return match([&] {
        auto c = ch();

        if (is_control_char(c) || c == '"' || c == '\\')
          return false;

        forward();

        return true;
      });
    };

    return match([&] {
      if (ch() != '"')
        return false;

      forward();

      while (parse_escape_sequence() || parse_noncontrol_char())
        ;

      if (ch() != '"')
        return false;

      forward();

      return true;
    });
  }

  bool Tokenizer::parse_operator() {
    // clang-format off
    constexpr const char* Operators[] = {
      "++",
      "--",
      "**",
      "==",
      "||",
      "&&",
      "*",
      "/",
      "+",
      "-",
      "=",
    };
    // clang-format on

    // dont actually need the match here since we dont move anywhere in a failing case
    // just keeping it for styles sake
    return match([&] {
      for (auto& str : Operators) {
        if (peek(0, strlen(str)) == str) {
          forward(strlen(str));
          return true;
        }
      }

      return false;
    });
  }

  bool Tokenizer::parse_while() {
    return match([&] {
      if (peek(0, 5) == "while") {
        forward(5);
        return true;
      }

      return false;
    });
  }

  bool Tokenizer::parse_if() {
    return match([&] {
      if (peek(0, 2) == "if") {
        forward(2);
        return true;
      }

      return false;
    });
  }

  bool Tokenizer::parse_elseif() {
    return match([&] {
      if (peek(0, 6) == "elseif") {
        forward(6);
        return true;
      }

      return false;
    });
  }

  bool Tokenizer::parse_else() {
    return match([&] {
      if (peek(0, 4) == "else") {
        forward(4);
        return true;
      }

      return false;
    });
  }

} // namespace Oxy::XSDL::Compiler
