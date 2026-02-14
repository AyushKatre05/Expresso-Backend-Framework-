#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <fstream>
#include <sstream>
#include <zlib.h>

constexpr char RESP_200[] = "HTTP/1.1 200 OK\r\n\r\n";
constexpr size_t RESP_200_LEN = sizeof(RESP_200) - 1;

constexpr char RESP_404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
constexpr size_t RESP_404_LEN = sizeof(RESP_404) - 1;

constexpr char RESP_201[] = "HTTP/1.1 201 Created\r\n\r\n";
constexpr size_t RESP_201_LEN = sizeof(RESP_201) - 1;


std::string g_directory = "";

std::string gzip_compress(const std::string& data) {
  z_stream zs;
  std::memset(&zs, 0, sizeof(zs));

  if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED,  15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
    return "";
  }

  zs.next_in = (Bytef*)data.data();
  zs.avail_in = data.size();

  int ret;
  char outbuffer[32768];
  std::string compressed;

  do {
    zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
    zs.avail_out = sizeof(outbuffer);

    ret = deflate(&zs, Z_FINISH);

    if (compressed.size() < zs.total_out) {
      compressed.append(outbuffer, zs.total_out - compressed.size());
    }
  } while (ret == Z_OK);

  deflateEnd(&zs);

  if (ret != Z_STREAM_END) {
    return "";
  }

  return compressed;
}

void send_http_response(const int& client_fd, const char* buf, const size_t& lenght) {
  size_t sent = 0;
  while (sent < lenght) {
    ssize_t cnt = ::send(client_fd, buf + sent, lenght - sent, 0);
    
    if (cnt > 0) {
      sent += static_cast<size_t>(cnt);
      continue;
    }

    if (cnt == 0) {
      std::cerr << "send() returned 0: conncetion may be closed\n";
      break;
    }
    
    if (errno == EINTR) {
      continue;
    }

    std::cerr << "send() failed: (" << errno << ") " << std::strerror(errno) << "\n";
    break; 
  }
}
void send_http_response(const int& client_fd, const std::string& buf) {
  size_t sent = 0;
  size_t lenght = buf.size();
  
  const char* data = buf.data();

  while (sent < lenght) {
    ssize_t cnt = ::send(client_fd, data + sent, lenght - sent, 0);
    
    if (cnt > 0) {
      sent += static_cast<size_t>(cnt);
      continue;
    }

    if (cnt == 0) {
      std::cerr << "send() returned 0: conncetion may be closed\n";
      break;
    }
    
    if (errno == EINTR) {
      continue;
    }

    std::cerr << "send() failed: (" << errno << ") " << std::strerror(errno) << "\n";
    break; 
  }
}
std::string extract_header_value(const char* buf, const size_t buf_len, const std::string& header_name);

bool read_http_request(const int& client_fd, char*& out, size_t& out_len) {
  out = nullptr;
  out_len = 0;

  const size_t MAX = 64 * 1024;
  size_t cap = 4096;

  char* data = (char*)std::malloc(cap + 1);
  if (!data) {
    return false;
  }

  size_t len = 0;
  data[0] = '\0';

  char chunk[4096];
  size_t search_from = 0;

  while (len < MAX) {
    ssize_t n = ::recv(client_fd, chunk, sizeof(chunk), 0);
    if (n > 0) {

      if (len + (size_t)n > cap) {
        while (cap < len + (size_t)n) {
          cap *= 2;
        }

        char* nd = (char*)std::realloc(data, cap + 1);
        if (!nd) {
          std::free(data);
          return false;
        }
        data = nd;
      }

      std::memcpy(data + len, chunk, (size_t)n);
      len += (size_t)n;
      data[len] = '\0';

      for (size_t i = search_from; i + 3 < len; ++ i) {
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
    if (errno == EINTR) {
      continue;
    }
    
    std::cerr << "recv() failed (errno=" << errno << "): " << std::strerror(errno) << "\n";
    std::free(data);
    return false;
  }

  std::free(data);
  return false;
}
std::string extract_route(const char* buf, const size_t buf_len) {
  std::string answer = "";
  for (size_t i = 0; i < buf_len; ++ i) {
    if (buf[i] == '/') {
      while (i < buf_len && buf[i] != ' ') {
        answer += buf[i];
        ++ i;
      }
      break;
    }
  }
  
  return answer;
}
std::string extract_header_value(const char* buf, const size_t buf_len, const std::string& header_name) {
  std::string search = header_name + ": ";
  
  for (size_t i = 0; i + search.size() < buf_len; ++i) {
    bool match = true;
    for (size_t j = 0; j < search.size(); ++j) {
      if (std::tolower(buf[i + j]) != std::tolower(search[j])) {
        match = false;
        break;
      }
    }
    
    if (match) {
      size_t start = i + search.size();
      std::string value = "";
      while (start < buf_len && buf[start] != '\r' && buf[start] != '\n') {
        value += buf[start];
        ++start;
      }
      return value;
    }
  }
  
  return "";
}
std::string extract_method(const char* buf, const size_t buf_len) {
  std::string method = "";
  for (size_t i = 0; i < buf_len && buf[i] != ' '; ++ i) {
    method += buf[i];
  }
  return method;
}
std::string extract_body(const char* buf, const size_t buf_len) {
  for (size_t i = 0; i + 3 < buf_len; ++ i) {
    if (buf[i] == '\r' && buf[i + 1] == '\n' && buf[i + 2] == '\r' && buf[i + 3] == '\n') {
      size_t body_start = i + 4;
      return std::string(buf + body_start, buf_len - body_start);
    }
  }
  return "";
}
bool write_file(const std::string& filepath, const std::string& content) {
  std::ofstream file(filepath, std::ios::binary);
  if (!file) {
    return false;
  }

  file << content;
  return file.good();
}
std::string read_file(const std::string& filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) {
    return "";
  }

  std::ostringstream contents;
  contents << file.rdbuf();
  return contents.str();
}
bool supports_gzip(const char* req, const size_t req_len) {
  std::string accept_encoding = extract_header_value(req, req_len, "Accept-Encoding");
  
  return accept_encoding.find("gzip") != std::string::npos;
}

bool should_close_connection(const char* req, const size_t req_len) {
  std::string connection = extract_header_value(req, req_len, "Connection");
  return connection.find("close") != std::string::npos;
}
void get_endpoint(const int& client_fd, const char* req, const size_t& req_len) {
  std::string route = extract_route(req, req_len);
  bool close_requested = should_close_connection(req, req_len);

  // std::cerr << route << "\n";
  if (route == "/") {
    if (close_requested) {
      std::string response = "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n";
      send_http_response(client_fd, response);
    } else {
      send_http_response(client_fd, RESP_200, RESP_200_LEN);
    }
    return;
  }

  if (route.size() > 6  && route.substr(0, 6) == "/echo/") {
    std::string response_body = route.substr(6, std::string::npos);
    
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n";
    
    bool use_gzip = supports_gzip(req, req_len);
    
    if (use_gzip) {
      response += "Content-Encoding: gzip\r\n";
      response_body = gzip_compress(response_body);
    }
    
    if (close_requested) {
      response += "Connection: close\r\n";
    }
    
    response += "Content-Length: " + std::to_string(response_body.size()) + "\r\n\r\n" + response_body;
    
    send_http_response(client_fd, response);
    return;
  }

  if (route == "/user-agent") {
    std::string user_agent = extract_header_value(req, req_len, "User-Agent");
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n";
    
    if (close_requested) {
      response += "Connection: close\r\n";
    }
    
    response += "Content-Length: " + std::to_string(user_agent.size()) + "\r\n\r\n" + user_agent;
    send_http_response(client_fd, response);
    return;
  }

  if (route.size() > 7 && route.substr(0, 7) == "/files/") {
    std::string filename = route.substr(7);
    std::string filepath = g_directory + "/" + filename;

    std::string file_contents = read_file(filepath);

    if (file_contents.empty()) {
      std::ifstream check (filepath);
      if (!check) {
        if (close_requested) {
          std::string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
          send_http_response(client_fd, response);
        } else {
          send_http_response(client_fd, RESP_404, RESP_404_LEN);
        }
        return;
      }
    }

    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n";
    
    if (close_requested) {
      response += "Connection: close\r\n";
    }
    
    response += "Content-Length: " + std::to_string(file_contents.size()) + "\r\n\r\n" + file_contents;
    send_http_response(client_fd, response); 
    return;
  }

  if (close_requested) {
    std::string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
    send_http_response(client_fd, response);
  } else {
    send_http_response(client_fd, RESP_404, RESP_404_LEN);
  }
}
void post_endpoint(const int& client_fd, const char* req, const size_t& req_len) {
  std::string route = extract_route(req, req_len);
  bool close_requested = should_close_connection(req, req_len);

  if (route.size() > 7 && route.substr(0, 7) == "/files/") {
    std::string filename = route.substr(7);
    std::string filepath = g_directory + "/" + filename;
    
    std::string body = extract_body(req, req_len);
    
    if (write_file(filepath, body)) {
      if (close_requested) {
        std::string response = "HTTP/1.1 201 Created\r\nConnection: close\r\n\r\n";
        send_http_response(client_fd, response);
      } else {
        send_http_response(client_fd, RESP_201, RESP_201_LEN);
      }
    } else {
      if (close_requested) {
        std::string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
        send_http_response(client_fd, response);
      } else {
        send_http_response(client_fd, RESP_404, RESP_404_LEN);
      }
    }
    return;
  }

  if (close_requested) {
    std::string response = "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n";
    send_http_response(client_fd, response);
  } else {
    send_http_response(client_fd, RESP_404, RESP_404_LEN);
  }
}
