#include "../expresso-types/ExpressHttp.h"
#include "../expresso-types/error.h"
#include "parser.h"
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

static int ascii_strcasecmp(const char *a, const char *b) {
  unsigned char ca, cb;
  while (*a && *b) {
    ca = (unsigned char)*a++;
    cb = (unsigned char)*b++;
    ca = (unsigned char)tolower(ca);
    cb = (unsigned char)tolower(cb);
    if (ca != cb) return (int)ca - (int)cb;
  }
  return (int)(unsigned char)tolower((unsigned char)*a) -
         (int)(unsigned char)tolower((unsigned char)*b);
}

static int has_header(const ExpressHeader *h, const char *key) {
  for (; h; h = h->next) {
    if (h->key && ascii_strcasecmp(h->key, key) == 0) return 1;
  }
  return 0;
}

ExpressStatus init_request(ExpressRequest **req) {
  *req = calloc(1, sizeof(ExpressRequest));
  if (!*req)
    return EXPRESS_PARSE_MEM_ERR;
  ExpressRequest *temp = *req;
  temp->method = NULL;
  temp->url = NULL;
  temp->httpVersion = NULL;
  temp->headers = NULL;
  temp->body = NULL;
  temp->bodyLength = 0;
  temp->timeout_ms = 0;
  temp->param = NULL;
  return EXPRESS_OK;
}

ExpressStatus set_request_method(ExpressRequest *req, const char *method) {
  if (is_valid_method(method) != EXPRESS_OK)
    return EXPRESS_PARSE_INVALID_METHOD;
  if (req->method)
    free(req->method);
  req->method = strdup(method);
  return EXPRESS_OK;
}

ExpressStatus set_request_url(ExpressRequest *req, const char *url) {
  if (req->url)
    free(req->url);
  req->url = strdup(url);
  return EXPRESS_OK;
}

ExpressStatus set_request_http_version(ExpressRequest *req,
                                       const char *httpVersion) {
  if (req->httpVersion)
    free(req->httpVersion);
  req->httpVersion = strdup(httpVersion);
  return EXPRESS_OK;
}

ExpressStatus set_request_timeout(ExpressRequest* req, unsigned int timeout) {
  req->timeout_ms = timeout;
  return EXPRESS_OK;
}

ExpressStatus add_request_header(ExpressRequest *req, const char *key,
                                 const char *value) {
  if (req->headers == NULL) {
    ExpressHeader *new_header = (ExpressHeader *)malloc(sizeof(ExpressHeader));
    new_header->key = strdup(key);
    new_header->value = strdup(value);
    new_header->next = NULL;
    req->headers = new_header;
  } else {
    ExpressHeader *new_header = (ExpressHeader *)malloc(sizeof(ExpressHeader));
    new_header->key = strdup(key);
    new_header->value = strdup(value);
    new_header->next = NULL;
    ExpressHeader *curr = req->headers;
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = new_header;
  }
  return EXPRESS_OK;
}

ExpressStatus set_request_body(ExpressRequest *req, const char *body,
                               size_t length) {
  if (req->body) {
    free(req->body);
  }
  req->body = strdup(body);
  req->bodyLength = length;
  return EXPRESS_OK;
}

ExpressStatus add_request_param(ExpressRequest *req, const char *key,
                                const char *value) {
  if (req->param == NULL) {
    Params *new_header = (Params *)malloc(sizeof(Params));
    new_header->key = strdup(key);
    new_header->value = strdup(value);
    new_header->next = NULL;
    req->param = new_header;
  } else {
    Params *new_header = (Params *)malloc(sizeof(Params));
    new_header->key = strdup(key);
    new_header->value = strdup(value);
    new_header->next = NULL;
    Params *curr = req->param;
    while (curr->next) {
      curr = curr->next;
    }
    curr->next = new_header;
  }
  return EXPRESS_OK;
}

ExpressStatus serialize_request(ExpressRequest *req, char *serialized, size_t *out_len) {
  if (!req || !serialized || !out_len) return EXPRESS_PARSE_REQUEST_ERROR;

  const char *method = req->method ? req->method : "GET";
  const char *httpv  = req->httpVersion ? req->httpVersion : "HTTP/1.1";
  const char *base_url = req->url ? req->url : "/";

  size_t off = 0;
  int n = 0;
  int url_has_q = (strchr(base_url, '?') != NULL);
  int has_params = (req->param != NULL);

  if (!has_params || url_has_q) {
    n = snprintf(serialized + off, MAX_BUFFER_SIZE - off, "%s %s %s\r\n",
                 method, base_url, httpv);
    if (n < 0) return EXPRESS_PARSE_REQUEST_ERROR;
    if ((size_t)n >= MAX_BUFFER_SIZE - off) return EXPRESS_PARSE_REQUEST_ERROR;
    off += (size_t)n;
  } else {
    n = snprintf(serialized + off, MAX_BUFFER_SIZE - off, "%s %s?",
                 method, base_url);
    if (n < 0) return EXPRESS_PARSE_REQUEST_ERROR;
    if ((size_t)n >= MAX_BUFFER_SIZE - off) return EXPRESS_PARSE_REQUEST_ERROR;
    off += (size_t)n;

    Params *p = req->param;
    while (p) {
      const char *k = p->key ? p->key : "";
      const char *v = p->value ? p->value : "";
      n = snprintf(serialized + off, MAX_BUFFER_SIZE - off, "%s=%s%s",
                   k, v, (p->next ? "&" : ""));
      if (n < 0) return EXPRESS_PARSE_REQUEST_ERROR;
      if ((size_t)n >= MAX_BUFFER_SIZE - off) return EXPRESS_PARSE_REQUEST_ERROR;
      off += (size_t)n;
      p = p->next;
    }

    n = snprintf(serialized + off, MAX_BUFFER_SIZE - off, " %s\r\n", httpv);
    if (n < 0) return EXPRESS_PARSE_REQUEST_ERROR;
    if ((size_t)n >= MAX_BUFFER_SIZE - off) return EXPRESS_PARSE_REQUEST_ERROR;
    off += (size_t)n;
  }

  for (ExpressHeader *h = req->headers; h; h = h->next) {
    if (!h->key || !h->value) continue;
    n = snprintf(serialized + off, MAX_BUFFER_SIZE - off, "%s: %s\r\n",
                 h->key, h->value);
    if (n < 0) return EXPRESS_PARSE_REQUEST_ERROR;
    if ((size_t)n >= MAX_BUFFER_SIZE - off) return EXPRESS_PARSE_REQUEST_ERROR;
    off += (size_t)n;
  }

  const char *body = req->body;
  size_t body_len = 0;

  if (body) {
    body_len = req->bodyLength ? req->bodyLength : strlen(body);

    if (!has_header(req->headers, "Content-Length")) {
      n = snprintf(serialized + off, MAX_BUFFER_SIZE - off,
                   "Content-Length: %zu\r\n", body_len);
      if (n < 0) return EXPRESS_PARSE_REQUEST_ERROR;
      if ((size_t)n >= MAX_BUFFER_SIZE - off) return EXPRESS_PARSE_REQUEST_ERROR;
      off += (size_t)n;
    }
  }

  n = snprintf(serialized + off, MAX_BUFFER_SIZE - off, "\r\n");
  if (n < 0) return EXPRESS_PARSE_REQUEST_ERROR;
  if ((size_t)n >= MAX_BUFFER_SIZE - off) return EXPRESS_PARSE_REQUEST_ERROR;
  off += (size_t)n;

  if (body && body_len > 0) {
    if (body_len > (MAX_BUFFER_SIZE - off - 1)) {
      return EXPRESS_PARSE_REQUEST_ERROR;
    }
    memcpy(serialized + off, body, body_len);
    off += body_len;
  }
  if (off >= MAX_BUFFER_SIZE) return EXPRESS_PARSE_REQUEST_ERROR;
  serialized[off] = '\0';
  *out_len = off;
  return EXPRESS_OK;
}
