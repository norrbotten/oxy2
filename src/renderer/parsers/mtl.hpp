#pragma once

#include <filesystem>
#include <fstream>
#include <vector>

#include "renderer/renderer.hpp"

namespace Oxy::Parsers {

  namespace fs = std::filesystem;

  bool parse_mtl(fs::path path, Renderer& renderer) {
    struct mtl_data {
      std::string name;

      double Ka[3];
      double Kd[3];
      double Ks[3];

      double d;
      double Tr; // 1 - d
    };

    std::ifstream infile(path, std::ios::binary);
    std::string   line;

    while (std::getline(infile, line)) {
    }

    return false;
  }

} // namespace Oxy::Parsers
