#pragma once

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "renderer/parsers/mtl.hpp"
#include "renderer/renderer.hpp"
#include "renderer/utils.hpp"

namespace Oxy::Parsers::OBJ {

  namespace fs = std::filesystem;

  namespace detail { // hiding this in an additional namespace

    struct Vertex {
      double x, y, z;
    };

    struct VertexTexture {
      double x, y;
    };

    struct VertexNormal {
      double x, y, z;
    };

    struct FacetFragment {
      int vertex{0};
      int texture{0};
      int normal{0};
    };

    struct Facet {
      Facet(const FacetFragment& f1, const FacetFragment& f2, const FacetFragment& f3)
          : vertex{f1.vertex, f2.vertex, f3.vertex}
          , texture{f1.texture, f2.texture, f3.texture}
          , normal{f1.normal, f2.normal, f3.normal} {}

      int vertex[3]{0};
      int texture[3]{0};
      int normal[3]{0};
    };

    struct Object {
      std::vector<Vertex>        vertices;
      std::vector<VertexTexture> vertex_textures;
      std::vector<VertexNormal>  vertex_normals;

      // (material name, facets...)
      std::unordered_map<std::string, std::vector<Facet>> facets;
    };

    template <typename... Args>
    auto parse_numbers = [](const std::string& line, Args&... args) {
      std::stringstream ss(line);
      (ss >> ... >> args);
    };

  } // namespace detail

  using ObjectFile = std::vector<detail::Object>;

  ObjectFile parse_obj(fs::path path) {
    using namespace detail;

    std::ifstream infile(path);
    std::string   line;

    ObjectFile          obj;
    MTL::MaterialBucket materials;

    std::string current_usemtl{"@"};

    while (std::getline(infile, line)) {
      if (line.at(0) == '#')
        continue;

      else if (line.substr(0, 6) == "mtllib")
        MTL::parse_mtl(path.parent_path() / line.substr(8), materials);

      else if (line.at(0) == 'o')
        obj.push_back(Object{});

      else if (line.at(0) == 'v') {
        if (obj.size() == 0)
          obj.push_back(Object{});

        line = line.substr(2);

        double x, y, z;
        parse_numbers<double, double, double>(line, x, y, z);
        obj.back().vertices.push_back(Vertex{x, y, z});
      }

      else if (line.substr(0, 2) == "vt") {
        if (obj.size() == 0)
          obj.push_back(Object{});

        line = line.substr(3);

        double x, y;
        parse_numbers<double, double>(line, x, y);
        obj.back().vertex_textures.push_back(VertexTexture{x, y});
      }

      else if (line.substr(0, 2) == "vn") {
        if (obj.size() == 0)
          obj.push_back(Object{});

        line = line.substr(3);

        double x, y, z;
        parse_numbers<double, double, double>(line, x, y, z);
        obj.back().vertex_normals.push_back(VertexNormal{x, y, z});
      }

      else if (line.substr(0, 6) == "usemtl")
        current_usemtl = line.substr(7);

      else if (line.at(0) == 'f') {
        if (obj.size() == 0)
          obj.push_back(Object{});

        auto facedecls = split(line.substr(2), " ");

        if (facedecls.size() < 3)
          continue;

        std::vector<FacetFragment> fragments; // each v1/t1/n1 is a fragment

        for (auto& face : facedecls) {
          auto parts = split(face, "/");

          int v{0}, t{0}, n{0};

          if (parts.size() == 1) {
            if (parts.at(0) != "")
              parse_numbers<int>(parts.at(0), v);
          }
          else if (parts.size() == 2) {
            if (parts.at(0) != "")
              parse_numbers<int>(parts.at(0), v);

            if (parts.at(1) != "")
              parse_numbers<int>(parts.at(1), t);
          }
          else if (parts.size() == 3) {
            if (parts.at(0) != "")
              parse_numbers<int>(parts.at(0), v);

            if (parts.at(1) != "")
              parse_numbers<int>(parts.at(1), t);

            if (parts.at(2) != "")
              parse_numbers<int>(parts.at(2), n);
          }

          fragments.push_back(FacetFragment{v, t, n});
        }

        auto& current_obj = obj.back();

        // dumb fan triangulation
        for (int i = 1; i < fragments.size() - 1; i++) {
          auto& first = fragments.at(0);
          auto& curr  = fragments.at(i);
          auto& next  = fragments.at(i + 1);

          current_obj.facets[current_usemtl].push_back(Facet{first, curr, next});
        }
      }
    }

    return obj;
  }

} // namespace Oxy::Parsers::OBJ
