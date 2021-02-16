#pragma once

#include <vector>

#include "renderer/xsdl/compiler/token.hpp"

namespace Oxy::XSDL::Compiler {

  enum class ParserState {
    Statement = 0,
    AnyExpression,
    LetStatement,
    FunctionDeclExpression,
    Error,
  };

  class Parser {
  public:
    Parser(std::vector<Token>& tokens)
        : m_tokens(tokens) {}

  private:
    bool shift();
    bool reduce();

    void push(const Token& token) { m_current_production.push_back(token); }
    void clear() { m_current_production.clear(); }

    void change_state(ParserState new_state) { m_state = new_state; }

    bool eos(std::size_t ofs = 0) const { return (m_position + ofs) >= m_tokens.size(); }

    TokenProxy token(std::size_t ofs = 0) {
      return eos(ofs) ? TokenProxy{} : m_tokens.at(m_position + ofs);
    }

    void forward(std::size_t num = 1) { m_position += num; }

  private:
    std::vector<Token> m_tokens;
    std::size_t        m_position{0};

    std::vector<Token> m_current_production;

    ParserState m_state{ParserState::Statement};
  };

} // namespace Oxy::XSDL::Compiler
