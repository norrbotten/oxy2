#include <iostream>

#include "renderer/netrender/httplib/tcp_client.hpp"

int main() {

  using namespace Oxy::NetRender;

  TCP::TCPClient client("127.0.0.1", 8080);

  std::cout << "bigbong\n";

  client.on_receive([](const char* data, std::size_t len) {
    std::cout << "Client received " << len << " bytes\n";
  });

  while (true) {
    if (client.state() == TCP::ClientState::CONNECTED) {
      std::cout << "sending\n";
      client.send("abcd", 4);
    }

    using namespace std::chrono_literals;
    std::this_thread::sleep_for(1s);
  }

  client.block();
}
