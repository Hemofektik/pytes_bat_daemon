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
utility::string_t Current{U("current")};
utility::string_t Energy{U("energy")};
utility::string_t EnergyChargeDaily{U("energy_charge_daily")};
utility::string_t EnergyDischargeDaily{U("energy_discharge_daily")};

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

auto asJson(const bms::AggregatedBatteryTelemetry& abt, 
            const bms::AccumulatedBatteryTelemetry& dailyAccBatTelemetry)
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

    if(abt.totalCurrent_A)
    {
        res[Current] = web::json::value::number(*abt.totalCurrent_A);
    }

    if(abt.totalEnergy_kWh)
    {
        res[Energy] = web::json::value::number(*abt.totalEnergy_kWh);
    }

    res[EnergyChargeDaily] = web::json::value::number(dailyAccBatTelemetry.energyCharged_kWh);
    res[EnergyDischargeDaily] = web::json::value::number(dailyAccBatTelemetry.energyDischarged_kWh);
    
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

RestService::RestService(const Config& config)
{   
    try
    {
        utility::string_t port{std::to_string(config.port)};
        utility::string_t address{U("http://")};
        address.append(config.address);
        address.append(U(":"));
        address.append(port);

        web::uri_builder uri(address);
        uri.append_path(BasePath);

        utility::string_t fullUriStr{uri.to_uri().to_string()};       
        std::cout << "Listening for requests at: " << fullUriStr << std::endl;

        service = std::make_unique<rest::Service>(fullUriStr, std::bind(&RestService::handleRequest, this, std::placeholders::_1, std::placeholders::_2));
        service->open().wait();
    }
    catch(const boost::wrapexcept<boost::system::system_error>& e)
    {
        throw std::runtime_error(e.what());
    }
}

RestService::~RestService()
{
    service->close().wait();
}

void RestService::updateBatteryTelemetry(const std::vector<bms::BatteryUnitTelemetry>& newBatteryTelemetry,
                                         const bms::AggregatedBatteryTelemetry& newAggregatedBatteryTelemetry,
                                         const bms::AccumulatedBatteryTelemetry& newDailyAccumulatedBatteryTelemetry)
{
    const int newLatestBatteryTelemetryIndex{(latestBatteryTelemetryIndex + 1) & 1};
    latestBatteryTelemetry[newLatestBatteryTelemetryIndex] = newBatteryTelemetry;
    latestAggregatedBatteryTelemetry[newLatestBatteryTelemetryIndex] = newAggregatedBatteryTelemetry;
    latestDailyAccumulatedBatteryTelemetry[newLatestBatteryTelemetryIndex] = newDailyAccumulatedBatteryTelemetry;
    latestBatteryTelemetryIndex = newLatestBatteryTelemetryIndex;
}

void RestService::handleRequest(const std::vector<utility::string_t>& paths, http_request& message)
{
    const auto& basePath{paths[0]};

    if (basePath == AggregatedPath)
    {
        message.reply(status_codes::OK, asJson(latestAggregatedBatteryTelemetry[latestBatteryTelemetryIndex], latestDailyAccumulatedBatteryTelemetry[latestBatteryTelemetryIndex]));
    }
    else
    {
        message.reply(status_codes::NotFound, asJson(paths));
    }
}

}