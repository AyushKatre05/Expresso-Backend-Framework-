#include "core/handlers.hpp"
#include "config/config.hpp"
#include "constants/http_constants.hpp"
#include "io/request_reader.hpp"
#include "io/response_sender.hpp"
#include "utils/compression.hpp"
#include "utils/file_utils.hpp"
#include "utils/request_utils.hpp"
#ifdef USE_EXPRESS_PARSER
#include "express/express_bridge.hpp"
#endif
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unistd.h>

namespace http {

void handle_get(int client_fd, const std::string& route, const char* req, size_t req_len) {
  bool close_requested = should_close_connection(req, req_len);

  if (route == "/") {
    if (close_requested) {
      send_response(client_fd, "HTTP/1.1 200 OK\r\nConnection: close\r\n\r\n");
    } else {
      send_response(client_fd, RESP_200, RESP_200_LEN);
    }
    return;
  }

  if (!docs_directory().empty() && (route == "/docs" || (route.size() >= 6 && route.substr(0, 6) == "/docs/"))) {
    std::string path = (route == "/docs" || route == "/docs/") ? "index.html" : route.substr(6);
    if (path.empty()) path = "index.html";
    if (path.find("..") != std::string::npos) {
      if (close_requested) {
        send_response(client_fd, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
      } else {
        send_response(client_fd, RESP_404, RESP_404_LEN);
      }
      return;
    }
    std::string filepath = docs_directory() + "/" + path;
    std::string file_contents = read_file(filepath);
    std::ifstream check(filepath);
    if (!check && file_contents.empty()) {
      if (close_requested) {
        send_response(client_fd, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
      } else {
        send_response(client_fd, RESP_404, RESP_404_LEN);
      }
      return;
    }
    std::string content_type = "application/octet-stream";
    if (path.size() >= 5 && path.compare(path.size() - 5, 5, ".html") == 0) content_type = "text/html; charset=utf-8";
    else if (path.size() >= 4 && path.compare(path.size() - 4, 4, ".css") == 0) content_type = "text/css; charset=utf-8";
    else if (path.size() >= 3 && path.compare(path.size() - 3, 3, ".js") == 0) content_type = "application/javascript; charset=utf-8";
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: " + content_type + "\r\n";
    if (close_requested) response += "Connection: close\r\n";
    response += "Content-Length: " + std::to_string(file_contents.size()) + "\r\n\r\n" + file_contents;
    send_response(client_fd, response);
    return;
  }

  if (route.size() > 6 && route.substr(0, 6) == "/echo/") {
    std::string response_body = route.substr(6, std::string::npos);
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n";
    if (supports_gzip(req, req_len)) {
      response += "Content-Encoding: gzip\r\n";
      response_body = gzip_compress(response_body);
    }
    if (close_requested) response += "Connection: close\r\n";
    response += "Content-Length: " + std::to_string(response_body.size()) + "\r\n\r\n" + response_body;
    send_response(client_fd, response);
    return;
  }

  if (route == "/user-agent") {
    std::string user_agent = extract_header_value(req, req_len, "User-Agent");
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n";
    if (close_requested) response += "Connection: close\r\n";
    response += "Content-Length: " + std::to_string(user_agent.size()) + "\r\n\r\n" + user_agent;
    send_response(client_fd, response);
    return;
  }

  if (route.size() > 7 && route.substr(0, 7) == "/files/") {
    std::string filename = route.substr(7);
    std::string filepath = directory() + "/" + filename;
    std::string file_contents = read_file(filepath);

    if (file_contents.empty()) {
      std::ifstream check(filepath);
      if (!check) {
        if (close_requested) {
          send_response(client_fd, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
        } else {
          send_response(client_fd, RESP_404, RESP_404_LEN);
        }
        return;
      }
    }

    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n";
    if (close_requested) response += "Connection: close\r\n";
    response += "Content-Length: " + std::to_string(file_contents.size()) + "\r\n\r\n" + file_contents;
    send_response(client_fd, response);
    return;
  }

  if (close_requested) {
    send_response(client_fd, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
  } else {
    send_response(client_fd, RESP_404, RESP_404_LEN);
  }
}

void handle_post(int client_fd, const std::string& route, const char* req, size_t req_len) {
  bool close_requested = should_close_connection(req, req_len);

  if (route.size() > 7 && route.substr(0, 7) == "/files/") {
    std::string filename = route.substr(7);
    std::string filepath = directory() + "/" + filename;
    std::string body = extract_body(req, req_len);

    if (write_file(filepath, body)) {
      if (close_requested) {
        send_response(client_fd, "HTTP/1.1 201 Created\r\nConnection: close\r\n\r\n");
      } else {
        send_response(client_fd, RESP_201, RESP_201_LEN);
      }
    } else {
      if (close_requested) {
        send_response(client_fd, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
      } else {
        send_response(client_fd, RESP_404, RESP_404_LEN);
      }
    }
    return;
  }

  if (close_requested) {
    send_response(client_fd, "HTTP/1.1 404 Not Found\r\nConnection: close\r\n\r\n");
  } else {
    send_response(client_fd, RESP_404, RESP_404_LEN);
  }
}

void handle_client(int client_fd) {
  while (true) {
    char* req = nullptr;
    size_t len_req = 0;

    if (!read_request(client_fd, req, len_req)) break;

    std::string method;
    std::string route;
#ifdef USE_EXPRESS_PARSER
    express::ExpressRequest* expr_req = express::parse_request(req, len_req);
    if (expr_req) {
      method = express::method_from_request(expr_req);
      route = express::path_from_url(expr_req);
      express::free_express_request(expr_req);
    } else {
      method = extract_method(req, len_req);
      route = extract_route(req, len_req);
    }
#else
    method = extract_method(req, len_req);
    route = extract_route(req, len_req);
#endif
    bool close_connection = should_close_connection(req, len_req);

    if (method == "GET") {
      handle_get(client_fd, route, req, len_req);
    } else if (method == "POST") {
      handle_post(client_fd, route, req, len_req);
    } else {
      send_response(client_fd, RESP_404, RESP_404_LEN);
    }

    if (req) std::free(req);
    if (close_connection) break;
  }
  ::close(client_fd);
  std::cout << "Client disconnected\n";
}

}  // namespace http
