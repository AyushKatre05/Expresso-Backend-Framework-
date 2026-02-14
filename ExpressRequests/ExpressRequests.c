#include "ExpressRequests.h"

#include "../ExpressTypes/ExpressHttp.h"

#include <arpa/inet.h>
#include <ctype.h>
#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int ascii_strcasecmp(const char *a, const char *b) {
  unsigned char ca, cb;
  while (*a && *b) {
    ca = (unsigned char)tolower((unsigned char)*a++);
    cb = (unsigned char)tolower((unsigned char)*b++);
    if (ca != cb) return (int)ca - (int)cb;
  }
  return (int)tolower((unsigned char)*a) - (int)tolower((unsigned char)*b);
}


static const char *find_header_value(const ExpressHeader *h, const char *key) {
  for (; h; h = h->next) {
    if (h->key && h->value && ascii_strcasecmp(h->key, key) == 0) return h->value;
  }
  return NULL;
}

static int has_header(const ExpressHeader *h, const char *key) {
  return find_header_value(h, key) != NULL;
}

static ExpressHeader *append_header(ExpressHeader **head,
                                    const char *key,
                                    const char *value) {
  ExpressHeader *n = (ExpressHeader *)malloc(sizeof(ExpressHeader));
  if (!n) return NULL;
  n->key = strdup(key ? key : "");
  n->value = strdup(value ? value : "");
  n->next = NULL;
  if (!n->key || !n->value) {
    free(n->key);
    free(n->value);
    free(n);
    return NULL;
  }
  if (!*head) {
    *head = n;
  } else {
    ExpressHeader *cur = *head;
    while (cur->next) cur = cur->next;
    cur->next = n;
  }
  return n;
}

static ExpressStatus init_response_fields(ExpressResponse *res) {
  if (!res) return EXPRESS_PARSE_REQUEST_ERROR;
  memset(res, 0, sizeof(*res));
  res->statusCode = 0;
  return EXPRESS_OK;
}

static void free_response_fields_local(ExpressResponse *res) {
  if (!res) return;
  free(res->method);
  free(res->url);
  free(res->statusMessage);
  if (res->headers) free_headers(res->headers);
  free(res->body);
  memset(res, 0, sizeof(*res));
}

static int send_all(int sockfd, const unsigned char *buf, size_t len) {
  size_t off = 0;
  while (off < len) {
    ssize_t n = send(sockfd, buf + off, len - off, 0);
    if (n < 0) {
      if (errno == EINTR) continue;
      return -1;
    }
    if (n == 0) return -1;
    off += (size_t)n;
  }
  return 0;
}


static int parse_url_host_port_path(const ExpressRequest *req,
                                    char **out_host,
                                    char **out_port,
                                    char **out_path) {
  *out_host = NULL;
  *out_port = NULL;
  *out_path = NULL;

  const char *url = req && req->url ? req->url : "/";
  const char *p = url;

  if (strncmp(p, "https://", 8) == 0) return -2;

  if (strncmp(p, "http://", 7) == 0) {
    p += 7;
    const char *host_begin = p;
    while (*p && *p != '/' && *p != '?' && *p != '#') p++;
    const char *host_end = p;

    const char *colon = memchr(host_begin, ':', (size_t)(host_end - host_begin));
    if (colon) {
      *out_host = strndup(host_begin, (size_t)(colon - host_begin));
      *out_port = strndup(colon + 1, (size_t)(host_end - (colon + 1)));
    } else {
      *out_host = strndup(host_begin, (size_t)(host_end - host_begin));
      *out_port = strdup("80");
    }
    if (!*out_host || !*out_port) return -1;

    *out_path = (*p) ? strdup(p) : strdup("/");
    if (!*out_path) return -1;
    return 0;
  }

  const char *hosthdr = find_header_value(req->headers, "Host");
  if (!hosthdr) return -3;

  while (*hosthdr == ' ' || *hosthdr == '\t') hosthdr++;
  const char *c = strchr(hosthdr, ':');
  if (c) {
    *out_host = strndup(hosthdr, (size_t)(c - hosthdr));
    *out_port = strdup(c + 1);
  } else {
    *out_host = strdup(hosthdr);
    *out_port = strdup("80");
  }
  if (!*out_host || !*out_port) return -1;

  *out_path = strdup(url[0] ? url : "/");
  if (!*out_path) return -1;
  return 0;
}

static int decode_chunked(const unsigned char *in, size_t in_len,
                          unsigned char **out, size_t *out_len) {
  *out = NULL;
  *out_len = 0;

  size_t cap = 0;
  unsigned char *buf = NULL;

  size_t i = 0;
  while (i < in_len) {
    size_t line_start = i;
    while (i + 1 < in_len && !(in[i] == '\r' && in[i + 1] == '\n')) i++;
    if (i + 1 >= in_len) { free(buf); return -1; }

    char tmp[64];
    size_t line_len = i - line_start;
    if (line_len >= sizeof(tmp)) { free(buf); return -1; }
    memcpy(tmp, in + line_start, line_len);
    tmp[line_len] = '\0';

    char *semi = strchr(tmp, ';');
    if (semi) *semi = '\0';

    errno = 0;
    unsigned long chunk_sz = strtoul(tmp, NULL, 16);
    if (errno != 0) { free(buf); return -1; }

    i += 2;

    if (chunk_sz == 0) {
      *out = buf ? buf : (unsigned char *)calloc(1, 1);
      return (*out != NULL) ? 0 : -1;
    }

    if (i + chunk_sz + 2 > in_len) { free(buf); return -1; }

    if (*out_len + (size_t)chunk_sz + 1 > cap) {
      size_t newcap = cap ? cap : 1024;
      while (newcap < *out_len + (size_t)chunk_sz + 1) newcap *= 2;
      unsigned char *nb = (unsigned char *)realloc(buf, newcap);
      if (!nb) { free(buf); return -1; }
      buf = nb;
      cap = newcap;
    }

    memcpy(buf + *out_len, in + i, (size_t)chunk_sz);
    *out_len += (size_t)chunk_sz;
    i += (size_t)chunk_sz;

    if (!(in[i] == '\r' && in[i + 1] == '\n')) { free(buf); return -1; }
    i += 2;
  }

  free(buf);
  return -1;
}