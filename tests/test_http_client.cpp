
#include "renderer/netrender/httplib/http_client.hpp"

int main() {
  using namespace Oxy::NetRender;

  HTTP::HTTPClient client(HTTP::Method::GET, "http://localhost:8080/");

  client.on_response(
      [](const HTTP::IncomingHTTPResponse& res) { std::cout << res.body() << "\n"; });

  client.send();
  client.block();
}
