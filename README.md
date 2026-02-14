# Expresso Backend Framework

Loosely coupled HTTP server and Express-style request/response types and parser.

## Folder structure

```
Expresso-Backend-Framework-
├── CMakeLists.txt                 # Top-level build (HTTP + Express)
├── README.md
├── ExpressTypes/                  # Shared C types
│   ├── ExpressHttp.h
│   └── error.h
├── ExpressParser/                 # C HTTP request/response parser
│   ├── parser.h
│   ├── parser.c
│   ├── request_parse.c
│   ├── request_builder.c
│   └── memory.c
├── ExpressRequests/               # C HTTP client (send request, parse response)
│   ├── ExpressRequests.h
│   └── ExpressRequests.c
└── HTTP/                          # C++ HTTP server
    ├── CMakeLists.txt
    └── src/
        ├── main.cpp
        ├── config/
        │   ├── config.hpp
        │   └── config.cpp
        ├── constants/
        │   └── http_constants.hpp
        ├── io/
        │   ├── request_reader.hpp
        │   ├── request_reader.cpp
        │   ├── response_sender.hpp
        │   └── response_sender.cpp
        ├── utils/
        │   ├── request_utils.hpp
        │   ├── request_utils.cpp
        │   ├── file_utils.hpp
        │   ├── file_utils.cpp
        │   ├── compression.hpp
        │   └── compression.cpp
        ├── core/
        │   ├── server.hpp
        │   ├── server.cpp
        │   ├── handlers.hpp
        │   └── handlers.cpp
        └── express/
            ├── express_bridge.hpp
            └── express_bridge.cpp
```

## Build

### HTTP server only (no Express)

```bash
cd HTTP
mkdir build && cd build
cmake ..
cmake --build .
./http-server [--directory /path/to/files]
```

### HTTP server + Express parser (from repo root)

```bash
mkdir build && cd build
cmake .. -DUSE_EXPRESS_PARSER=ON
cmake --build .
./http-server [--directory /path/to/files]
```

To build without linking Express: `cmake .. -DUSE_EXPRESS_PARSER=OFF`

Requires: C++23, pthreads, zlib. POSIX (Linux/WSL/macOS) for sockets.
