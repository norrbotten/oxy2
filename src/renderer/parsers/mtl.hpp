#pragma once

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>

namespace Oxy::Parsers::MTL {

  namespace fs = std::filesystem;

  struct Material {
    std::string name;

    double Ka[3];
    double Kd[3];
    double Ks[3];

    double d;
    double Tr; // 1 - d
  };

  using MaterialBucket = std::unordered_map<std::string, Material>;

  bool parse_mtl(fs::path path, MaterialBucket& result) {
    std::ifstream infile(path, std::ios::binary);
    std::string   line;

    while (std::getline(infile, line)) {
    }

    return false;
  }

} // namespace Oxy::Parsers::MTL
