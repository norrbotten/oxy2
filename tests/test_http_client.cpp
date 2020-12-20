
#include "renderer/netrender/httplib/http_client.hpp"

int main() {
  using namespace Oxy::NetRender;

  HTTP::HTTPClient client(HTTP::Method::GET, "http://google.com/");

  client.on_response([] { std::cout << "brumm\n"; });
  client.send();
  client.block();

  std::cout << "fucked\n";
}
