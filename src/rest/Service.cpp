#include <rest/Service.h>
#include "cpprest/asyncrt_utils.h"
#include "cpprest/json.h"
#include "cpprest/uri.h"
#include <thread>
#include <chrono>

using namespace web;
using namespace http;
using namespace utility;
using namespace http::experimental::listener;

namespace pytes::rest
{

Service::Service(utility::string_t url, RequestHandler requestHandlerParam) 
: listener(url)
, requestHandler{requestHandlerParam}
{
    assert(requestHandler);
    listener.support(methods::GET, std::bind(&Service::handleGet, this, std::placeholders::_1));
}

void Service::handleGet(http_request message)
{
    //ucout << message.to_string() << std::endl;

    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    if (paths.empty())
    {
        web::json::value result = web::json::value::object();
        message.reply(status_codes::OK, result);
        return;
    }

    requestHandler(paths, message);
};


}