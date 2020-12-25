#pragma once

#include <optional>

#include "renderer/common/ast_builder.hpp"
#include "renderer/common/parser.hpp"

#include "renderer/xsdl/compiler/ast/ast.hpp"

namespace Oxy::XSDL::Compiler {

  using ASTBuilder = Common::ASTBuilder<ASTNode>;

  using Token = std::optional<std::string>;

  class Parser final : public Common::Parser<ASTBuilder> {
  public:
    Parser(const std::string& input)
        : Common::Parser<ASTBuilder>(input) {}

    Token parse_identifier();
    Token parse_integer_literal();
    Token parse_decimal_literal();

    Token parse_keyword();
    Token parse_operator();

    Token parse_left_parenthesis();
    Token parse_right_parenthesis();
    Token parse_left_bracket();
    Token parse_right_bracket();
    Token parse_left_sqbracket();
    Token parse_right_sqbracket();

    bool match_identifier();
    bool match_integer_literal();
    bool match_decimal_literal();

    bool match_left_parenthesis();
    bool match_right_parenthesis();

    bool match_literal_expression();

    bool match_expression();
  };

} // namespace Oxy::XSDL::Compiler
