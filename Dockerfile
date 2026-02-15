# Expresso backend - build and run the HTTP server
FROM ubuntu:22.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    cmake \
    zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /src
COPY expresso-types ./expresso-types
COPY expresso-parser ./expresso-parser
COPY expresso-server ./expresso-server
COPY CMakeLists.txt .

WORKDIR /src/build
RUN find /src -name "express_bridge.hpp"
RUN cmake .. -DUSE_EXPRESS_PARSER=ON && cmake --build . -j$(nproc)

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    libstdc++6 \
    zlib1g \
    ca-certificates \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /src/build/expresso-server /app/expresso-server
COPY docs /app/docs

# Default: serve files from /data (override with --directory)
ENV EXPRESSO_DIRECTORY=/data
RUN mkdir -p /data

EXPOSE 4221

CMD ["/app/expresso-server", "--directory", "/data", "--docs", "/app/docs"]
