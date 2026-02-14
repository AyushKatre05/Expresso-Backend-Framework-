#ifndef HANDLERS_HPP
#define HANDLERS_HPP

#include <cstddef>
#include <string>

namespace http {

void handle_get(int client_fd, const std::string& route, const char* req, size_t req_len);
void handle_post(int client_fd, const std::string& route, const char* req, size_t req_len);
void handle_client(int client_fd);

}  // namespace http

#endif
