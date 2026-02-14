#include "config/config.hpp"

namespace http {

static std::string g_directory;

std::string& directory() { return g_directory; }
void set_directory(const std::string& dir) { g_directory = dir; }

} 
