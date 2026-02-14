/* Implementation split into:
 *   request_parse.c - parsing (parse_http_request, parse_headers, etc.)
 *   request_builder.c - request builder (init_request, set_request_*, serialize_request)
 *   memory.c - cleanup (free_headers, free_request, free_response, etc.)
 * This file is kept for reference; build using the split sources.
 */

#include "parser.h"
