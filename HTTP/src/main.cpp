#include <iostream>
#include <cstdlib>
#include <cerrno>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <thread>
#include <fstream>
#include <sstream>
#include <zlib.h>

constexpr char RESP_200[] = "HTTP/1.1 200 OK\r\n\r\n";
constexpr size_t RESP_200_LEN = sizeof(RESP_200) - 1;

constexpr char RESP_404[] = "HTTP/1.1 404 Not Found\r\n\r\n";
constexpr size_t RESP_404_LEN = sizeof(RESP_404) - 1;

constexpr char RESP_201[] = "HTTP/1.1 201 Created\r\n\r\n";
constexpr size_t RESP_201_LEN = sizeof(RESP_201) - 1;


std::string g_directory = "";

std::string gzip_compress(const std::string& data) {
  z_stream zs;
  std::memset(&zs, 0, sizeof(zs));

  if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED,  15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
    return "";
  }

  zs.next_in = (Bytef*)data.data();
  zs.avail_in = data.size();

  int ret;
  char outbuffer[32768];
  std::string compressed;

  do {
    zs.next_out = reinterpret_cast<Bytef*>(outbuffer);
    zs.avail_out = sizeof(outbuffer);

    ret = deflate(&zs, Z_FINISH);

    if (compressed.size() < zs.total_out) {
      compressed.append(outbuffer, zs.total_out - compressed.size());
    }
  } while (ret == Z_OK);

  deflateEnd(&zs);

  if (ret != Z_STREAM_END) {
    return "";
  }

  return compressed;
}