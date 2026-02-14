#ifndef REQUEST_UTILS_HPP
#define REQUEST_UTILS_HPP

#include <cstddef>
#include <string>

namespace http {

std::string extract_header_value(const char* buf, size_t buf_len, const std::string& header_name);
std::string extract_route(const char* buf, size_t buf_len);
std::string extract_method(const char* buf, size_t buf_len);
std::string extract_body(const char* buf, size_t buf_len);
bool supports_gzip(const char* req, size_t req_len);
bool should_close_connection(const char* req, size_t req_len);

}  // namespace http

#endif
