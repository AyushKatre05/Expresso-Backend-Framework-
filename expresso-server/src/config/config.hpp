#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>

namespace http {

std::string& directory();
void set_directory(const std::string& dir);

std::string docs_directory();
void set_docs_directory(const std::string& dir);

}  // namespace http

#endif
