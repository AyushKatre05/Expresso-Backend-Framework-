#ifndef REQUEST_READER_HPP
#define REQUEST_READER_HPP

#include <cstddef>

namespace http {

bool read_request(int client_fd, char*& out, size_t& out_len);

}  // namespace http

#endif
