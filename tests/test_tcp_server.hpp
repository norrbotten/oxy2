#include <iostream>

#include "renderer/netrender/httplib/tcp_server.hpp"

int main() {
  using namespace Oxy::NetRender;

  TCP::TCPServer server(8080);

  server.on_connection([](TCP::Connection* conn) {
    std::cout << "New connection fd=" << conn->fd() << "\n";

    conn->write("hawwo uwu", 9);

    conn->on_receive([](TCP::Connection* client, const void* data, std::size_t len) {
      client->write(data, len);
    });
  });

  server.block();
}
