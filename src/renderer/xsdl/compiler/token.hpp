#pragma once

#include <cassert>
#include <sstream>
#include <string>

namespace Oxy::XSDL::Compiler {

  // clang-format off
  enum class TokenType {
    EndOfStatement,
    BooleanConstant,
    NumberConstant,
    StringConstant,
    Operator,
    While,
    For,
    If,
    ElseIf,
    Else,
    Break,
    Continue,
    OtherKeyword,
    Parenthesis,
    Bracket,
    SquigglyBracket,
    Identifier,
  };

  constexpr const char* TokenTypeString[] = {
    "EndOfStatement",
    "BooleanConstant",
    "NumberConstant",
    "StringConstant",
    "Operator",
    "While",
    "For",
    "If",
    "ElseIf",
    "Else",
    "Break",
    "Continue",
    "OtherKeyword",
    "Parenthesis",
    "Bracket",
    "SquigglyBracket",
    "Identifier",
  };
  // clang-format on

  struct Token {
    TokenType   type;
    std::string literal;

    int line{1}, column{1};

    std::string stringify() const {
      std::stringstream ss;

      ss << "Token [type=" << TokenTypeString[int(type)] << ", ";

      if (literal.size() < 16)
        ss << "literal=" << literal << " ";
      else
        ss << "literal(" << literal.size() << ") ";

      ss << line << ":" << column << "]";

      return ss.str();
    }
  };

  class TokenProxy {
  public:
    TokenProxy() = default;
    TokenProxy(const Token& token)
        : m_token(token)
        , m_token_valid(true) {}

    auto& token() const { return m_token; }
    bool  valid() const { return m_token_valid; }

    operator bool() { return m_token_valid; }

    auto operator->() {
      assert(m_token_valid);
      return &m_token;
    }

  private:
    Token m_token;
    bool  m_token_valid{false};
  };

} // namespace Oxy::XSDL::Compiler
