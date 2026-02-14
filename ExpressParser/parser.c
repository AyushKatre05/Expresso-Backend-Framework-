#include "../ExpressTypes/ExpressHttp.h"

#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ExpressStatus is_valid_method(const char *method) {
  const char *valid_methods[] = {"GET",   "POST", "PUT",     "DELETE",
                                 "PATCH", "HEAD", "OPTIONS", NULL};
  for (int i = 0; valid_methods[i] != NULL; i++) {
    if (strcmp(method, valid_methods[i]) == 0) {
      return EXPRESS_OK;
    }
  }
  return EXPRESS_PARSE_INVALID_METHOD;
}

ExpressStatus parse_http_request(char *raw_request, size_t length,
                                 ExpressRequest *expr_req) {
  HttpRequest *req = malloc(sizeof(HttpRequest));
  char *req_ptr = raw_request;
  char *tempreq = strstr(raw_request, "\r\n");
  ExpressStatus frontlinestatus =
      parse_request_line(raw_request, tempreq - raw_request, req);
  if (frontlinestatus != EXPRESS_OK) {
    PARSE_FAIL(req, frontlinestatus);
  }
  req_ptr += tempreq - raw_request + 2;
  tempreq = strstr(req_ptr, "\r\n\r\n");
  ExpressStatus header_status = parse_headers(req_ptr, tempreq - req_ptr, req);
  if (header_status != EXPRESS_OK) {
    PARSE_FAIL(req, header_status);
  }
  req_ptr += tempreq - req_ptr + 4;
  ExpressStatus body_status =
      parse_body(req_ptr, length - (req_ptr - raw_request), req);
  if (body_status != EXPRESS_OK) {
    PARSE_FAIL(req, body_status);
  }

  if (is_valid_method(req->method) != EXPRESS_OK) {
    // cleanup here
  }
  expr_req->url = req->url;
  expr_req->method = req->method;
  expr_req->httpVersion = req->httpVersion;
  expr_req->headers = req->headers;
  expr_req->body = req->body;
  expr_req->bodyLength = strlen(req->body);
  expr_req->timeout_ms = 0;
  expr_req->param = NULL;
  parse_request_params(expr_req->url, strlen(expr_req->url), expr_req);
  free(req);
  return EXPRESS_OK;
}
