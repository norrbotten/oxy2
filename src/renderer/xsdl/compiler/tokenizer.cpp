#include "renderer/xsdl/compiler/tokenizer.hpp"

namespace Oxy::XSDL::Compiler {

  void Tokenizer::process() {
    m_tokens.clear();

    while (!eof()) {
      skip_whitespace();
      m_streamer.discard();

      if (parse_boolean())
        push_token(TokenType::BooleanConstant);
      else if (parse_number())
        push_token(TokenType::NumberConstant);
      else if (parse_string())
        push_token(TokenType::StringConstant);
      else
        throw TokenizationError("Unknown syntax");
    }
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

} // namespace Oxy::XSDL::Compiler
