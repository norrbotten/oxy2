#pragma once

#include <exception>
#include <string>

#include "renderer/sdl/ast.hpp"
#include "renderer/sdl/utils.hpp"

namespace Oxy::SDL {

  class ParsingError final : public std::exception {
  public:
    ParsingError(std::string err)
        : m_error(err) {}

    virtual const char* what() const noexcept { return m_error.c_str(); }

  private:
    std::string m_error;
  };

  class Parser {
  public:
    Parser(const std::string& input)
        : m_input(input)
        , m_position(0)
        , m_consume_ptr(0) {}

    SceneDeclarationNode* parse() {
      try {
        return parse_scene_declaration();
      }
      catch (const ParsingError& e) {
        std::cout << e.what() << "\n";
        return nullptr;
      }
    }

  private:
    bool match_declaration_start(std::string decl);

    bool unexpected_eof(std::string what = "") {
      if (eof())
        throw ParsingError(fmt([&](auto& ss) {
          ss << "Unexpected EOF";
          if (what.size() > 0)
            ss << " while " << what;
        }));

      return false;
    }

    std::string parse_key_name();
    std::string parse_value_literal();
    std::string parse_nested_bracket_literal();

    SceneDeclarationNode* parse_scene_declaration();

    TextureDeclarationNode*  parse_texture_declaration();
    MaterialDeclarationNode* parse_material_declaration();
    ObjectDeclarationNode*   parse_object_declaration();

  private:
    void pop_state() {
      m_position    = m_state_stack.back().position;
      m_consume_ptr = m_state_stack.back().consume_ptr;
      m_state_stack.pop_back();
    }
    void push_state() { m_state_stack.push_back({m_position, m_consume_ptr}); }
    void clear_state_stack() { m_state_stack.clear(); }

    void skip_whitespace() {
      while (is_newline(ch()) || is_whitespace(ch()))
        forward();
    }

    void forward(int step = 1) { m_position += step; }

    bool eof(int offset = 0) { return (m_position + offset) >= m_input.size(); }

    char ch(int offset = 0) {
      if (eof(offset))
        return '0';

      return m_input.at(m_position + offset);
    }

    std::string peek(int offset = 0, int length = 1) {
      if (eof(offset + length))
        return "";

      return m_input.substr(m_position + offset, length);
    }

    bool match(std::string against) { return peek(0, against.size()) == against; }

    std::string consume() {
      if (m_position == m_consume_ptr)
        return "";

      auto str      = m_input.substr(m_consume_ptr, m_position - m_consume_ptr);
      m_consume_ptr = m_position;

      return str;
    }

  private:
    std::string m_input;

    int m_position, m_consume_ptr;

    struct State {
      int position, consume_ptr;
    };

    std::vector<State> m_state_stack;
  };

} // namespace Oxy::SDL