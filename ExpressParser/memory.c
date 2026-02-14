#include "../ExpressTypes/ExpressHttp.h"
#include "../ExpressTypes/error.h"
#include "parser.h"
#include <stdlib.h>
#include <string.h>

ExpressStatus free_headers(ExpressHeader *header) {
  while (header) {
    ExpressHeader *temp = header;
    header = header->next;
    free(temp->key);
    free(temp->value);
    free(temp);
  }
  return EXPRESS_OK;
}

ExpressStatus free_params(Params *param) {
  while (param) {
    Params *temp = param;
    param = param->next;
    free(temp->key);
    free(temp->value);
    free(temp);
  }
  return EXPRESS_OK;
}

ExpressStatus free_http_request(HttpRequest *req) {
  if (req->method)
    free(req->method);
  if (req->url)
    free(req->url);
  if (req->httpVersion)
    free(req->httpVersion);
  if (req->headers)
    free_headers(req->headers);
  if (req->body)
    free(req->body);
  return EXPRESS_OK;
}

ExpressStatus free_request(ExpressRequest *req) {
  if (req->method)
    free(req->method);
  if (req->url)
    free(req->url);
  if (req->httpVersion)
    free(req->httpVersion);
  if (req->headers)
    free_headers(req->headers);
  if (req->body)
    free(req->body);
  if (req->param)
    free_params(req->param);
  free(req);
  return EXPRESS_OK;
}

ExpressStatus free_http_response(HttpResponse *res) {
  if (res->statusMessage)
    free(res->statusMessage);
  if (res->headers)
    free_headers(res->headers);
  if (res->body)
    free(res->body);
  return EXPRESS_OK;
}

ExpressStatus free_response(ExpressResponse *res) {
  if (res->method)
    free(res->method);
  if (res->url)
    free(res->url);
  if (res->statusMessage)
    free(res->statusMessage);
  if (res->headers)
    free_headers(res->headers);
  if (res->body)
    free(res->body);
  free(res);
  return EXPRESS_OK;
}
