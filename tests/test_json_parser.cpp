#include "renderer/netrender/jsonlib/parser.hpp"

int main() {
  using namespace Oxy::NetRender;

  JSON::Parser parser(
      "3.141592653589793238462643383279502884197169399375105820974944592307816406286e-5");
  parser.parse();
}
