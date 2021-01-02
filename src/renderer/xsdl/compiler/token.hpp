#pragma once

#include <sstream>
#include <string>

namespace Oxy::XSDL::Compiler {

  // clang-format off
  enum class TokenType {
    BooleanConstant,
    NumberConstant,
    StringConstant,
    Operator,
    While,
    For,
    If,
    IfElse,
    Else,
    Keyword,
  };

  constexpr const char* TokenTypeString[] = {
    "BooleanConstant",
    "NumberConstant",
    "StringConstant",
    "Operator",
    "While",
    "For",
    "If",
    "IfElse",
    "Else",
    "Keyword",
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

} // namespace Oxy::XSDL::Compiler
