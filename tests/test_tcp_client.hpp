#include <iostream>

#include "renderer/netrender/httplib/tcp_client.hpp"

int main() {

  using namespace Oxy::NetRender;

  TCP::TCPClient client("localhost", 8080);

  client.on_receive([](const char* data, std::size_t len) {

  });

  client.block();
}
