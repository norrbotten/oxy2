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
    auto forward(int steps = 1) { return m_streamer.forward(steps); }
    auto discard() { return m_streamer.discard(); }

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
      auto [literal, diff] = m_streamer.consume();
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

    bool parse_number();
    bool parse_string();

  private:
    Streamer           m_streamer;
    std::vector<Token> m_tokens;

    int m_line{1}, m_column{1};
  };

} // namespace Oxy::XSDL::Compiler
