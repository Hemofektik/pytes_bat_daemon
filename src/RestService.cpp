#include "RestService.h"
#include "cpprest/uri.h"

namespace pytes
{

RestService::RestService()
{
    utility::string_t fullUriStr;
    try
    {
        utility::string_t port = U("7735");
        utility::string_t address = U("http://localhost:");
        address.append(port);

        web::uri_builder uri(address);
        uri.append_path(U("power"));

        fullUriStr = uri.to_uri().to_string();
        service = std::make_unique<rest::Service>(fullUriStr);
        service->open().wait();
    }
    catch(const boost::wrapexcept<boost::system::system_error>& e)
    {
        throw std::runtime_error(e.what());
    }

    std::cout << "Listening for requests at: " << fullUriStr << std::endl;
}

RestService::~RestService()
{
    service->close().wait();
}

}