#ifndef HTTP_CONSTANTS_HPP
#define HTTP_CONSTANTS_HPP

namespace http {

constexpr char RESP_200[] = "HTTP/1.1 200 OK\r\n\r\n";
constexpr size_t RESP_200_LEN = sizeof(RESP_200) - 1;

constexpr char RESP_404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
constexpr size_t RESP_404_LEN = sizeof(RESP_404) - 1;

constexpr char RESP_201[] = "HTTP/1.1 201 Created\r\n\r\n";
constexpr size_t RESP_201_LEN = sizeof(RESP_201) - 1;

}  // namespace http

#endif
