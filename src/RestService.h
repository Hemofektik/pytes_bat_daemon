#include <rest/Service.h>

namespace pytes
{


class RestService
{

public:
    RestService();

private:
    std::unique_ptr<rest::Service> service;
};



}