#include "renderer/sdl/parser.hpp"

// TODO: refactor.

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
    if (unexpected_eof("parsing key name") || !is_alphanumeric(ch()))
      return "";

    while (unexpected_eof("parsing key name") || is_alphanumeric(ch()))
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
    KeyValue nested_params;

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

          if (key_name == "params")
            nested_params = Parser::parse_shallow_keyvals(value_literal);
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node = new TextureDeclarationNode();

    node->decl()          = parsed_vals;
    node->nested_params() = nested_params;

    return node;
  }

  ObjectDeclarationNode* Parser::parse_object_declaration() {
    KeyValue parsed_vals;
    KeyValue nested_params;

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

          if (key_name == "params")
            nested_params = Parser::parse_shallow_keyvals(value_literal);
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node = new ObjectDeclarationNode();

    node->decl()          = parsed_vals;
    node->nested_params() = nested_params;

    return node;
  }

  MaterialDeclarationNode* Parser::parse_material_declaration() {
    KeyValue parsed_vals;
    KeyValue nested_params;

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

          if (key_name == "params")
            nested_params = Parser::parse_shallow_keyvals(value_literal);
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node = new MaterialDeclarationNode();

    node->decl()          = parsed_vals;
    node->nested_params() = nested_params;

    return node;
  }

  CameraDeclarationNode* Parser::parse_camera_declaration() {
    KeyValue parsed_vals;

    while (unexpected_eof("parsing camera declaration") || true) {
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
          throw ParsingError(
              fmt([&](auto& ss) { ss << "Unexpected nested params in camera declaration"; }));
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node = new CameraDeclarationNode();

    node->decl() = parsed_vals;

    return node;
  }

  RenderDeclarationNode* Parser::parse_render_declaration() {
    KeyValue parsed_vals;

    while (unexpected_eof("parsing render declaration") || true) {
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
          throw ParsingError(
              fmt([&](auto& ss) { ss << "Unexpected nested params in render declaration"; }));
        }
        else {
          consume();
          auto value_literal = parse_value_literal();
          parsed_vals.emplace(key_name, value_literal);
        }
      }
    }

    auto node = new RenderDeclarationNode();

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
      else if (match_declaration_start("camera")) {
        if (m_parsed_camera_decl)
          throw ParsingError(fmt(
              [&](auto& ss) { ss << "Unexpected camera declaration, one was already parsed"; }));

        if (auto node = parse_camera_declaration(); node != nullptr) {
          root->push(node);
          m_parsed_camera_decl = true;
        }
      }
      else if (match_declaration_start("render")) {
        if (m_parsed_render_decl)
          throw ParsingError(fmt(
              [&](auto& ss) { ss << "Unexpected render declaration, one was already parsed"; }));

        if (auto node = parse_render_declaration(); node != nullptr) {
          root->push(node);
          m_parsed_render_decl = true;
        }
      }
      else {
      }
    }

    return root;
  }

  KeyValue Parser::parse_keyvals() {
    if (m_input.size() == 0)
      return {};

    assert(ch() == '{');
    forward(1);

    skip_whitespace();
    consume();

    KeyValue parsed_vals;

    while (unexpected_eof("parsing keyvalue") || true) {
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

    return parsed_vals;
  }

} // namespace Oxy::SDL
