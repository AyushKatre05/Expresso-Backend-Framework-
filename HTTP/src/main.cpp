#include "config/config.hpp"
#include "core/server.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
  std::cout << std::unitbuf;
  std::cerr << std::unitbuf;

  for (int i = 1; i < argc; ++i) {
    if (std::string(argv[i]) == "--directory" && i + 1 < argc) {
      http::set_directory(argv[i + 1]);
      ++i;
    }
  }

  std::cout << "Logs from your program will appear here!\n";
  return run_server(4221);
}
