#include "utils/file_utils.hpp"
#include <fstream>
#include <sstream>

namespace http {

bool write_file(const std::string& filepath, const std::string& content) {
  std::ofstream file(filepath, std::ios::binary);
  if (!file) return false;
  file << content;
  return file.good();
}

std::string read_file(const std::string& filepath) {
  std::ifstream file(filepath, std::ios::binary);
  if (!file) return "";
  std::ostringstream contents;
  contents << file.rdbuf();
  return contents.str();
}

}  // namespace http
