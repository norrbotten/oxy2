
#include "renderer/netrender/httplib/http_server.hpp"

int main() {
  using namespace Oxy::NetRender;

  HTTP::HTTPServer server(8080);

  server.set_404_handler([](auto, auto res) { res.write_body("404").finalize().send(); });

  server.get("/", [](auto, auto res) {
    res.write_body("indexbong");
    res.finalize().send();
  });

  server.block();
}
