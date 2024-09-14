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

Service::Service(utility::string_t url) : m_listener(url)
{
    m_listener.support(methods::GET, std::bind(&Service::handleGet, this, std::placeholders::_1));
}

void Service::handleGet(http_request message)
{
    ucout << message.to_string() << std::endl;

    auto paths = http::uri::split_path(http::uri::decode(message.relative_uri().path()));
    if (paths.empty())
    {
        //web::json::value result = web::json::value::array();
        web::json::value result = web::json::value::object();

        message.reply(status_codes::OK, result);
        return;
    }

    //utility::string_t wtable_id = paths[0];

    // Get information on a specific table.
    /*auto found = s_tables.find(table_id);
    if (found == s_tables.end())
    {
        message.reply(status_codes::NotFound);
    }
    else
    {
        message.reply(status_codes::OK, found->second->AsJSON());
    }*/
};


}