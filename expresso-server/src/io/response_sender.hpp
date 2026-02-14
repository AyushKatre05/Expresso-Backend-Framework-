#ifndef RESPONSE_SENDER_HPP
#define RESPONSE_SENDER_HPP

#include <cstddef>
#include <string>

namespace http {

void send_response(int client_fd, const char* buf, size_t length);
void send_response(int client_fd, const std::string& buf);

}  // namespace http

#endif
