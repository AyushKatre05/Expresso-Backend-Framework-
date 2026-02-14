#include "io/request_reader.hpp"
#include "utils/request_utils.hpp"
#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sys/socket.h>

namespace http {

bool read_request(int client_fd, char*& out, size_t& out_len) {
  out = nullptr;
  out_len = 0;
  const size_t MAX = 64 * 1024;
  size_t cap = 4096;

  char* data = static_cast<char*>(std::malloc(cap + 1));
  if (!data) return false;

  size_t len = 0;
  data[0] = '\0';
  char chunk[4096];
  size_t search_from = 0;

  while (len < MAX) {
    ssize_t n = ::recv(client_fd, chunk, sizeof(chunk), 0);
    if (n > 0) {
      if (len + static_cast<size_t>(n) > cap) {
        while (cap < len + static_cast<size_t>(n)) cap *= 2;
        char* nd = static_cast<char*>(std::realloc(data, cap + 1));
        if (!nd) {
          std::free(data);
          return false;
        }
        data = nd;
      }
      std::memcpy(data + len, chunk, static_cast<size_t>(n));
      len += static_cast<size_t>(n);
      data[len] = '\0';

      for (size_t i = search_from; i + 3 < len; ++i) {
        if (data[i] == '\r' && data[i + 1] == '\n' && data[i + 2] == '\r' && data[i + 3] == '\n') {
          size_t headers_end = i + 4;
          std::string content_length_str = extract_header_value(data, len, "Content-Length");
          size_t content_length = 0;
          if (!content_length_str.empty()) {
            content_length = std::stoull(content_length_str);
          }
          size_t total_needed = headers_end + content_length;
          if (len >= total_needed) {
            out = data;
            out_len = total_needed;
            return true;
          }
          if (content_length == 0) {
            out = data;
            out_len = headers_end;
            return true;
          }
          break;
        }
      }
      search_from = (len >= 3 ? (len - 3) : 0);
      continue;
    }
    if (n == 0) {
      std::free(data);
      return false;
    }
    if (errno == EINTR) continue;
    std::cerr << "recv() failed (errno=" << errno << "): " << std::strerror(errno) << "\n";
    std::free(data);
    return false;
  }
  std::free(data);
  return false;
}

}  // namespace http
