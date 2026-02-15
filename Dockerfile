# Builder stage
FROM rust:1.75 as builder

WORKDIR /usr/src/expresso
COPY expresso-parser ../expresso-parser
COPY expresso-rs .
RUN cargo build --release

# Runtime stage
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY --from=builder /usr/src/expresso/target/release/expresso /app/expresso
COPY docs /app/docs

ENV EXPRESSO_DIRECTORY=/data
RUN mkdir -p /data

EXPOSE 4221

CMD ["/app/expresso"]
