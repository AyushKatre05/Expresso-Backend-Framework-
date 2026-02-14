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