#include "renderer/sdl/parser.hpp"

namespace Oxy::SDL {

  bool Parser::match_declaration_start(std::string decl) {
    if (match(decl)) {
      push_state();

      forward(decl.size());
      skip_whitespace();

      if (match(":")) {
        push_state();

        forward(1);
        skip_whitespace();

        if (match("{")) {
          clear_state_stack();

          forward(1);
          return true;
        }
        else {
          pop_state();
          return false;
        }
      }
      else {
        pop_state();
        return false;
      }
    }
    else {
      return false;
    }
  }

  std::string Parser::parse_key_name() {
    if (unexpected_eof("parsing key name") || !is_alpha(ch()))
      return "";

    while (unexpected_eof("parsing key name") || is_alpha(ch()))
      forward();

    return consume();
  }

  std::string Parser::parse_value_literal() {
    while (unexpected_eof("parsing value literal") || !is_newline(ch()))
      forward();

    return consume();
  }

  std::string Parser::parse_nested_bracket_literal() {
    assert(ch() == '{');

    forward(1);

    int level = 1;
    while (unexpected_eof("parsing nested bracket literal") || level > 0) {
      if (ch() == '{')
        level++;
      if (ch() == '}')
        level--;

      forward(1);
    }

    return consume();
  }

  TextureDeclarationNode* Parser::parse_texture_declaration() {
    KeyValue parsed_vals;

    while (unexpected_eof("parsing texture declaration") || true) {
      skip_whitespace();

      if (ch() == '}') {
        forward();
        break;
      }
      else {
        consume();
        auto key_name = parse_key_name();

        if (key_name.size() == 0)
          throw ParsingError(fmt([&](auto& ss) {
            ss << "Expected key name, got ";
            ss << ch();
          }));

        skip_whitespace();

        if (ch() != ':')
          throw ParsingError(fmt([&](auto& ss) {
            ss << "Expected ':' after key name, got ";
            ss << ch();
          }));

        forward(1);
        skip_whitespace();

        if (ch() == '{') {
          consume();
          auto value_literal = parse_nested_bracket_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node    = new TextureDeclarationNode();
    node->decl() = parsed_vals;
    return node;
  }

  ObjectDeclarationNode* Parser::parse_object_declaration() {
    KeyValue parsed_vals;

    while (unexpected_eof("parsing object declaration") || true) {
      skip_whitespace();

      if (ch() == '}') {
        forward();
        break;
      }
      else {
        consume();
        auto key_name = parse_key_name();

        if (key_name.size() == 0)
          throw ParsingError(fmt([&](auto& ss) {
            ss << "Expected key name, got ";
            ss << ch();
          }));

        skip_whitespace();

        if (ch() != ':')
          throw ParsingError(fmt([&](auto& ss) {
            ss << "Expected ':' after key name, got ";
            ss << ch();
          }));

        forward(1);
        skip_whitespace();

        if (ch() == '{') {
          consume();
          auto value_literal = parse_nested_bracket_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node    = new ObjectDeclarationNode();
    node->decl() = parsed_vals;
    return node;
  }

  MaterialDeclarationNode* Parser::parse_material_declaration() {
    KeyValue parsed_vals;

    while (unexpected_eof("parsing material declaration") || true) {
      skip_whitespace();

      if (ch() == '}') {
        forward();
        break;
      }
      else {
        consume();
        auto key_name = parse_key_name();

        if (key_name.size() == 0)
          throw ParsingError(fmt([&](auto& ss) {
            ss << "Expected key name, got ";
            ss << ch();
          }));

        skip_whitespace();

        if (ch() != ':')
          throw ParsingError(fmt([&](auto& ss) {
            ss << "Expected ':' after key name, got ";
            ss << ch();
          }));

        forward(1);
        skip_whitespace();

        if (ch() == '{') {
          consume();
          auto value_literal = parse_nested_bracket_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node    = new MaterialDeclarationNode();
    node->decl() = parsed_vals;
    return node;
  }

  SceneDeclarationNode* Parser::parse_scene_declaration() {
    auto root = new SceneDeclarationNode();

    while (!eof()) {
      skip_whitespace();
      consume();

      if (match_declaration_start("texture")) {
        if (auto node = parse_texture_declaration(); node != nullptr) {
          root->push(node);
        }
      }
      else if (match_declaration_start("material")) {
        if (auto node = parse_material_declaration(); node != nullptr) {
          root->push(node);
        }
      }
      else if (match_declaration_start("object")) {
        if (auto node = parse_object_declaration(); node != nullptr) {
          root->push(node);
        }
      }
      else {
      }
    }

    return root;
  }

} // namespace Oxy::SDL
