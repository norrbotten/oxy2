#pragma once

#include <vector>

#include "renderer/utils.hpp"

#include "renderer/xsdl/compiler/streamer.hpp"
#include "renderer/xsdl/compiler/token.hpp"

namespace Oxy::XSDL::Compiler {

  class TokenizationError final : public std::exception {
  public:
    TokenizationError(const std::string& error)
        : m_error(error) {}

    TokenizationError(const std::string& error, int line, int col) {
      std::stringstream ss;
      ss << error << " @ " << line << ":" << col;
      m_error = ss.str();
    }

    TokenizationError(const std::string& error, int line, int col, const std::string& expecting) {
      std::stringstream ss;
      ss << error << " @ line " << line << ":" << col << ", expecting " << expecting;
      m_error = ss.str();
    }

    virtual const char* what() const noexcept { return m_error.c_str(); }

  private:
    std::string m_error;
  };

  class Tokenizer {
  public:
    Tokenizer(const std::string& input)
        : m_streamer(input) {}

    void process();

    auto&       tokens() const { return m_tokens; }
    const auto& ctokens() const { return m_tokens; }

  private:
    auto ch() { return m_streamer.ch(); }
    auto eof() { return m_streamer.eof(); }
    auto peek(unsigned int o = 0, unsigned int l = 1) { return m_streamer.peek(o, l); }
    auto forward(int steps = 1) { return m_streamer.forward(steps); }
    auto discard() { return m_streamer.discard(); }

    auto token() const { return m_streamer.get_parsed(); }

    void skip_whitespace() {
      while (!eof()) {
        if (ch() == ' ' || ch() == '\t') {
          m_column++;
          forward();
        }
        else if (ch() == '\n' || ch() == '\r') {
          m_column = 1;
          m_line++;
          forward();
        }
        else
          break;
      }
    }

    void push_token(TokenType type) {
      /* clang-format off */
      const auto valid_keywords = {
        "let",
        "const",
        "func",
      };
      /* clang-format on */

      auto [literal, diff] = m_streamer.consume();

      if (type == TokenType::Identifier) {
        for (auto& keyword : valid_keywords) {
          if (literal == keyword)
            type = TokenType::OtherKeyword;
        }
      }

      m_tokens.push_back(Token{type, literal, m_line, m_column});

      m_column += diff + literal.size();
    }

    template <typename MatchFunctor>
    bool match(MatchFunctor functor) {
      auto [pos, consume] = m_streamer.getpos();

      if (!functor()) {
        m_streamer.setpos(pos, consume);
        return false;
      }

      return true;
    }

    bool parse_identifier_or_keyword();

    bool parse_end_of_statement();

    bool parse_boolean();
    bool parse_number();
    bool parse_string();

    bool parse_operator();

    bool parse_keyword(const char* word);

    bool parse_while();
    bool parse_for();
    bool parse_if();
    bool parse_elseif();
    bool parse_else();
    bool parse_break();
    bool parse_continue();

    bool parse_parenthesis();
    bool parse_brackets();
    bool parse_squiggly_brackets();

  private:
    Streamer           m_streamer;
    std::vector<Token> m_tokens;

    int m_line{1}, m_column{1};
  };

} // namespace Oxy::XSDL::Compiler
