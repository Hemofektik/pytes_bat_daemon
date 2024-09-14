#include "cpprest/http_listener.h"

namespace pytes::rest
{

class Service
{
public:
    Service(utility::string_t url);

    pplx::task<void> open() { return m_listener.open(); }
    pplx::task<void> close() { return m_listener.close(); }

private:
    void handleGet(web::http::http_request message);

    web::http::experimental::listener::http_listener m_listener;
};


}