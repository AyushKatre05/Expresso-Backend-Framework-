#ifndef COMPRESSION_HPP
#define COMPRESSION_HPP

#include <string>

namespace http {

std::string gzip_compress(const std::string& data);

}  // namespace http

#endif
