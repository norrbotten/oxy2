#pragma once

#include <filesystem>
#include <fstream>
#include <unordered_map>
#include <vector>

namespace Oxy::Parsers::MTL {

  namespace fs = std::filesystem;

  struct Material {
    std::string name;

    double Ns{0};

    double Ka[3]{0};
    double Kd[3]{0};
    double Ks[3]{0};
    double Ke[3]{0};

    double Ni{1.45};

    double d;
  };

  using MaterialBucket = std::unordered_map<std::string, Material>;

  bool parse_mtl(fs::path path, MaterialBucket& result) {
    std::ifstream infile(path, std::ios::binary);
    std::string   line;

    Material current_material;
    bool     first_material = true;

    while (std::getline(infile, line)) {
      if (line.at(0) == '#')
        continue;

      else if (line.substr(0, 6) == "newmtl") {
        if (!first_material)
          result[current_material.name] = current_material;

        if (first_material)
          first_material = false;

        current_material      = Material{};
        current_material.name = line.substr(7);
      }
    }

    return false;
  }

} // namespace Oxy::Parsers::MTL
