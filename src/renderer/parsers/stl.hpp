#pragma once

#include <fstream>
#include <optional>
#include <vector>

#include "renderer/primitives/triangle.hpp"

namespace Oxy::Parsers {

  bool parse_stl(const char* filename, std::vector<Primitive::TrianglePrimitive>& result) {
    struct stl_header {
      uint8_t  header[80];
      uint32_t num_tris;
    } __attribute__((packed));

    struct stl_vertex {
      float x, y, z;
    } __attribute__((packed));

    struct stl_triangle {
      float      nx, ny, nz;
      stl_vertex vert0;
      stl_vertex vert1;
      stl_vertex vert2;
      uint16_t   attrib;
    } __attribute__((packed));

    std::ifstream infile(filename, std::ios::binary);

    if (!infile.good())
      return true;

    stl_header header;

    infile.read((char*)&header, sizeof(header));

    for (unsigned int i = 0; i < header.num_tris && !infile.eof(); i++) {
      stl_triangle tri;
      infile.read((char*)&tri, sizeof(tri));

      Vec3 p0{tri.vert0.x, tri.vert0.y, tri.vert0.z};
      Vec3 p1{tri.vert1.x, tri.vert1.y, tri.vert1.z};
      Vec3 p2{tri.vert2.x, tri.vert2.y, tri.vert2.z};

      result.push_back(Primitive::TrianglePrimitive(p0, p1, p2));
    }

    return false;
  }

} // namespace Oxy::Parsers
