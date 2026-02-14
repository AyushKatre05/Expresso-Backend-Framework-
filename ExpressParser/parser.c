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