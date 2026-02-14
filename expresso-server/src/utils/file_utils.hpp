#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <string>

namespace http {

bool write_file(const std::string& filepath, const std::string& content);
std::string read_file(const std::string& filepath);

}  // namespace http

#endif
