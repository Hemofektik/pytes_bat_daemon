#include "RestService.h"
#include "cpprest/uri.h"

namespace pytes
{

RestService::RestService()
{
    utility::string_t port = U("7735");
    utility::string_t address = U("http://localhost:");
    address.append(port);

    web::uri_builder uri(address);
    uri.append_path(U("power"));

    auto addr = uri.to_uri().to_string();
    service = std::make_unique<rest::Service>(addr);
    service->open().wait();

    std::cout << "Listening for requests at: " << addr << std::endl;
}


}