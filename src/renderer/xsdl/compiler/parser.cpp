#include "renderer/xsdl/compiler/parser.hpp"

#include <cassert>

namespace Oxy::XSDL::Compiler {

  bool Parser::shift() {
    switch (m_state) {
    case ParserState::Statement: {
      if (token() && token()->type == TokenType::OtherKeyword && token()->literal == "let") {
        push(token().get());
        forward();
        change_state(ParserState::LetStatement);
      }
      else {
        return false;
      }
    } break;

    case ParserState::LetStatement: {
      if (token() && token()->type == TokenType::Identifier) {
        push(token().get());
        forward();

        // let x;
        if (token() && token()->type == TokenType::EndOfStatement) {
          push(token().get());
          forward();
          change_state(ParserState::Statement);
          return true;
        }
        // let x =
        else if (token() && token()->type == TokenType::Operator && token()->literal == "=") {
          push(token().get());
          forward();
          change_state(ParserState::AnyExpression);
          return true;
        }
      }
    } break;

    case ParserState::AnyExpression: {
      // 1, "a", true, etc..
      if (token() && (token()->type == TokenType::NumberConstant ||
                      token()->type == TokenType::StringConstant ||
                      token()->type == TokenType::BooleanConstant)) {
        push(token().get());
        forward();

        if (token() && token()->type == TokenType::EndOfStatement) {
          push(token().get());
          forward();

          change_state(ParserState::Statement);

          return true;
        }

        return false;
      }
      // any identifier
      else if (token() && token()->type == TokenType::Identifier) {
        push(token().get());
        forward();

        // identifier;
        if (token() && token()->type == TokenType::EndOfStatement) {
          push(token().get());
          forward();

          change_state(ParserState::Statement);

          return true;
        }
      }
    } break;

    case ParserState::FunctionDeclExpression:
    case ParserState::Error: assert(!"bork");
    }

    return true;
  }

  bool Parser::reduce() { return true; }

} // namespace Oxy::XSDL::Compiler
