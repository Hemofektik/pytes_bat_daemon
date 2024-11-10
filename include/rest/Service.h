#pragma once
#include "cpprest/http_listener.h"
#include <functional>

namespace pytes::rest
{

class Service
{
public:
    typedef std::function<void(const std::vector<utility::string_t>& paths, web::http::http_request& message)> RequestHandler;

    Service(utility::string_t url, RequestHandler requestHandlerParam);

    pplx::task<void> open() { return listener.open(); }
    pplx::task<void> close() { return listener.close(); }

private:
    void handleGet(web::http::http_request message);

    web::http::experimental::listener::http_listener listener;
    RequestHandler requestHandler;
};


}