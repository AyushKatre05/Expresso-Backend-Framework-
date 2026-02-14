# Expresso

Expresso is a loosely coupled HTTP backend with an Express-style request/response parser. It includes a C++ HTTP server, C parser library, and optional Nginx reverse proxy for production-style deployment.

## Folder structure

```
Expresso-Backend-Framework-/
├── CMakeLists.txt              # Top-level build (Expresso server + parser)
├── Dockerfile                  # Multi-stage build for expresso-server image
├── docker-compose.yml          # Run Expresso server + Nginx
├── .dockerignore
├── README.md
│
├── expresso-types/             # Shared C types (headers)
│   ├── ExpressHttp.h
│   └── error.h
│
├── expresso-parser/            # C HTTP request/response parser
│   ├── parser.h
│   ├── parser.c
│   ├── request_parse.c
│   ├── request_builder.c
│   └── memory.c
│
├── expresso-requests/          # C HTTP client (send request, parse response)
│   ├── ExpressRequests.h
│   └── ExpressRequests.c
│
├── expresso-server/            # C++ HTTP server
│   ├── CMakeLists.txt
│   ├── vcpkg.json
│   ├── vcpkg-configuration.json
│   └── src/
│       ├── main.cpp
│       ├── config/
│       │   ├── config.hpp
│       │   └── config.cpp
│       ├── constants/
│       │   └── http_constants.hpp
│       ├── io/
│       │   ├── request_reader.hpp
│       │   ├── request_reader.cpp
│       │   ├── response_sender.hpp
│       │   └── response_sender.cpp
│       ├── utils/
│       │   ├── request_utils.hpp
│       │   ├── request_utils.cpp
│       │   ├── file_utils.hpp
│       │   ├── file_utils.cpp
│       │   ├── compression.hpp
│       │   └── compression.cpp
│       ├── core/
│       │   ├── server.hpp
│       │   ├── server.cpp
│       │   ├── handlers.hpp
│       │   └── handlers.cpp
│       └── express/
│           ├── express_bridge.hpp
│           └── express_bridge.cpp
│
└── nginx/                      # Nginx config for Docker
    └── nginx.conf
```

## Run with Docker (recommended)

Uses **Nginx** as reverse proxy on port 80; **Expresso server** runs behind it on 4221.

**Requirements:** Docker and Docker Compose.

```bash
# Build and start (Nginx on port 80, Expresso server behind it)
docker compose up --build

# Or in background
docker compose up -d --build
```

- **http://localhost/** — All traffic is proxied to the Expresso server.
- Files for `/files/` routes are stored in a Docker volume; to add files:
  - Copy into the volume:  
    `docker cp ./myfile.txt expresso-server:/data/`
  - Or mount a host folder in `docker-compose.yml` under `expresso-server`:
    ```yaml
    volumes:
      - ./my-files:/data
    ```

Stop:

```bash
docker compose down
```

## Run locally (without Docker)

**Requirements:** CMake 3.13+, C++23 compiler, pthreads, zlib (e.g. `zlib1g-dev` on Debian/Ubuntu). POSIX (Linux, WSL, or macOS).

### From repo root (server + parser)

```bash
mkdir build && cd build
cmake .. -DUSE_EXPRESS_PARSER=ON
cmake --build .
./expresso-server --directory /path/to/files
```

Server listens on **port 4221**.

### Expresso server only (no parser)

```bash
cd expresso-server
mkdir build && cd build
cmake ..
cmake --build .
./expresso-server [--directory /path/to/files]
```

## Endpoints

| Method | Path | Description |
|--------|------|-------------|
| GET | `/` | 200 OK |
| GET | `/echo/<text>` | Returns `<text>`, supports gzip |
| GET | `/user-agent` | Returns `User-Agent` header |
| GET | `/files/<name>` | Serves file from `--directory` |
| POST | `/files/<name>` | Writes body to file under `--directory` |

## License

See repository.
