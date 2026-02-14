#include "utils/request_utils.hpp"
#include <cctype>

namespace http {

std::string extract_route(const char* buf, size_t buf_len) {
  std::string answer;
  for (size_t i = 0; i < buf_len; ++i) {
    if (buf[i] == '/') {
      while (i < buf_len && buf[i] != ' ') {
        answer += buf[i];
        ++i;
      }
      break;
    }
  }
  return answer;
}

std::string extract_header_value(const char* buf, size_t buf_len, const std::string& header_name) {
  std::string search = header_name + ": ";
  for (size_t i = 0; i + search.size() < buf_len; ++i) {
    bool match = true;
    for (size_t j = 0; j < search.size(); ++j) {
      if (std::tolower(static_cast<unsigned char>(buf[i + j])) != std::tolower(static_cast<unsigned char>(search[j]))) {
        match = false;
        break;
      }
    }
    if (match) {
      size_t start = i + search.size();
      std::string value;
      while (start < buf_len && buf[start] != '\r' && buf[start] != '\n') {
        value += buf[start];
        ++start;
      }
      return value;
    }
  }
  return "";
}

std::string extract_method(const char* buf, size_t buf_len) {
  std::string method;
  for (size_t i = 0; i < buf_len && buf[i] != ' '; ++i) {
    method += buf[i];
  }
  return method;
}

std::string extract_body(const char* buf, size_t buf_len) {
  for (size_t i = 0; i + 3 < buf_len; ++i) {
    if (buf[i] == '\r' && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n') {
      size_t body_start = i + 4;
      return std::string(buf + body_start, buf_len - body_start);
    }
  }
  return "";
}

bool supports_gzip(const char* req, size_t req_len) {
  std::string accept_encoding = extract_header_value(req, req_len, "Accept-Encoding");
  return accept_encoding.find("gzip") != std::string::npos;
}

bool should_close_connection(const char* req, size_t req_len) {
  std::string connection = extract_header_value(req, req_len, "Connection");
  return connection.find("close") != std::string::npos;
}

}  // namespace http
