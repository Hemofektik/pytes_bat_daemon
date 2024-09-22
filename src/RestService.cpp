#include "RestService.h"
#include "cpprest/uri.h"

namespace pytes
{

using namespace web::http;

namespace {

utility::string_t BasePath{U("power")};
utility::string_t AggregatedPath{U("aggregated")};
utility::string_t State{U("state")};
utility::string_t StateOfCharge{U("soc")};
utility::string_t Power{U("power")};

std::string toString(const bms::BatteryState& batState)
{
    switch (batState)
    {
    case bms::BatteryState::Unknown:
        return "Unknown";
    case bms::BatteryState::Absent:
        return "Absent";
    case bms::BatteryState::Charging:
        return "Charging";
    case bms::BatteryState::Discharging:
        return "Discharging";
    case bms::BatteryState::Idle:
        return "Idle";
    }

    throw std::runtime_error("Encountered unhandled battery state");
}

auto asJson(const bms::AggregatedBatteryTelemetry abt)
{
    web::json::value res = web::json::value::object();
    res[State] = web::json::value::string(toString(abt.baseState));

    if(abt.avgCoulomb_percent)
    {
        res[StateOfCharge] = web::json::value::number(*abt.avgCoulomb_percent);
    }

    if(abt.totalPower_W)
    {
        res[Power] = web::json::value::number(*abt.totalPower_W);
    }

    return res;
}

auto asJson(const std::vector<utility::string_t>& paths)
{
    web::json::value res = web::json::value::array();
    for ( size_t n{0}; n < paths.size(); n++ )
    {
        res[n] = web::json::value::string(paths[n]);
    }
    return res;
}

}

RestService::RestService()
{
    utility::string_t fullUriStr;
    try
    {
        utility::string_t port = U("7735");
        utility::string_t address = U("http://localhost:");
        address.append(port);

        web::uri_builder uri(address);
        uri.append_path(BasePath);

        fullUriStr = uri.to_uri().to_string();
        service = std::make_unique<rest::Service>(fullUriStr, std::bind(&RestService::handleRequest, this, std::placeholders::_1, std::placeholders::_2));
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

void RestService::updateBatteryTelemetry(const std::vector<bms::BatteryUnitTelemetry>& newBatteryTelemetry,
                                         const bms::AggregatedBatteryTelemetry& newAggregatedBatteryTelemetry)
{
    const int newLatestBatteryTelemetryIndex{(latestBatteryTelemetryIndex + 1) & 1};
    latestBatteryTelemetry[newLatestBatteryTelemetryIndex] = newBatteryTelemetry;
    latestAggregatedBatteryTelemetry[newLatestBatteryTelemetryIndex] = newAggregatedBatteryTelemetry;
    latestBatteryTelemetryIndex = newLatestBatteryTelemetryIndex;
}

void RestService::handleRequest(const std::vector<utility::string_t>& paths, http_request& message)
{
    const auto& basePath{paths[0]};

    if (basePath == AggregatedPath)
    {
        message.reply(status_codes::OK, asJson(latestAggregatedBatteryTelemetry[latestBatteryTelemetryIndex]));
    }
    else
    {
        message.reply(status_codes::NotFound, asJson(paths));
    }
}

}