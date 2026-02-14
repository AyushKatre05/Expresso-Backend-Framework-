#pragma once

#include "../expresso-types/ExpressHttp.h"
#include "../expresso-types/error.h"

ExpressResponse send_req(ExpressRequest* req);

ExpressPromise* send_req_async(ExpressRequest* req);
ExpressResponse* req_await(ExpressPromise* promise);