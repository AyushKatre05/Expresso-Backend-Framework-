#include "core/server.hpp"
#include "core/handlers.hpp"
#include <arpa/inet.h>
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

namespace {

int run_server_impl(int port) {
  int server_fd = ::socket(AF_INET, SOCK_STREAM, 0);
  if (server_fd < 0) {
    std::cerr << "Failed to create server socket\n";
    return 1;
  }
  int reuse = 1;
  if (::setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0) {
    std::cerr << "setsockopt failed\n";
    ::close(server_fd);
    return 1;
  }

  struct sockaddr_in server_addr;
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(static_cast<uint16_t>(port));

  if (::bind(server_fd, reinterpret_cast<struct sockaddr*>(&server_addr), sizeof(server_addr)) != 0) {
    std::cerr << "Failed to bind to port " << port << "\n";
    ::close(server_fd);
    return 1;
  }

  const int connection_backlog = 5;
  if (::listen(server_fd, connection_backlog) != 0) {
    std::cerr << "listen failed\n";
    ::close(server_fd);
    return 1;
  }

  std::cout << "Server is listening on port " << port << "...\n";
  while (true) {
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    int client_fd = ::accept(server_fd, reinterpret_cast<struct sockaddr*>(&client_addr), &client_addr_len);

    if (client_fd < 0) {
      std::cerr << "Failed to accept client connection\n";
      continue;
    }
    std::cout << "Client connected\n";
    std::thread client_thread(http::handle_client, client_fd);
    client_thread.detach();
  }
  ::close(server_fd);
  return 0;
}

}  // namespace

int run_server(int port) {
  return run_server_impl(port);
}
