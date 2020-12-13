#include "renderer/renderer.hpp"

#include <fstream>
#include <streambuf>

namespace Oxy {

  std::optional<std::string> Renderer::load_file(fs::path filename) {
    std::ifstream f(filename);

    if (!f.good()) {
      return "File not found/invalid";
    }

    std::string str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    auto parser = SDL::Parser(str);

    SDL::SceneDeclarationNode* scene_decl;

    try {
      scene_decl = parser.parse();
    }
    catch (const SDL::ParsingError& err) {
      return std::string("Parsing error: ") + err.what();
    }

    auto ctx = SDL::ExecutionContext();

    try {
      scene_decl->exec(ctx);
    }
    catch (const SDL::ExecutionError& err) {
      return std::string("Exec error: ") + err.what();
    }

    try {
      ctx.validate();
    }
    catch (const SDL::ValidationError& err) {
      return std::string("Validation error: ") + err.what();
    }

    build_scene_from_valid_ctx(ctx);

    return {};
  }

  void Renderer::build_scene_from_valid_ctx(const SDL::ExecutionContext& ctx) {}

} // namespace Oxy