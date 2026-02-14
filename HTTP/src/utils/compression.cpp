#include "utils/compression.hpp"
#include <cstring>
#include <zlib.h>

namespace http {

std::string gzip_compress(const std::string& data) {
  z_stream zs;
  std::memset(&zs, 0, sizeof(zs));

  if (deflateInit2(&zs, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
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
  if (ret != Z_STREAM_END) return "";
  return compressed;
}

}  // namespace http
