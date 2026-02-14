#include "io/response_sender.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

namespace http {

void send_response(int client_fd, const char* buf, size_t length) {
  size_t sent = 0;
  while (sent < length) {
    ssize_t cnt = ::send(client_fd, buf + sent, length - sent, 0);
    if (cnt > 0) {
      sent += static_cast<size_t>(cnt);
      continue;
    }
    if (cnt == 0) {
      std::cerr << "send() returned 0: connection may be closed\n";
      break;
    }
    if (errno == EINTR) continue;
    std::cerr << "send() failed: (" << errno << ") " << std::strerror(errno) << "\n";
    break;
  }
}

void send_response(int client_fd, const std::string& buf) {
  size_t sent = 0;
  size_t length = buf.size();
  const char* data = buf.data();
  while (sent < length) {
    ssize_t cnt = ::send(client_fd, data + sent, length - sent, 0);
    if (cnt > 0) {
      sent += static_cast<size_t>(cnt);
      continue;
    }
    if (cnt == 0) {
      std::cerr << "send() returned 0: connection may be closed\n";
      break;
    }
    if (errno == EINTR) continue;
    std::cerr << "send() failed: (" << errno << ") " << std::strerror(errno) << "\n";
    break;
  }
}

}  // namespace http
