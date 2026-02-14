#include "config/config.hpp"

namespace http {

static std::string g_directory;
static std::string g_docs_directory;

std::string& directory() { return g_directory; }
void set_directory(const std::string& dir) { g_directory = dir; }

std::string docs_directory() { return g_docs_directory; }
void set_docs_directory(const std::string& dir) { g_docs_directory = dir; }

} 
