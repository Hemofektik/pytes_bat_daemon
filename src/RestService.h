#include <rest/Service.h>
#include <bms/Telemetry.h>
#include <atomic>

namespace pytes
{


class RestService
{

public:
    RestService();
    ~RestService();

    void updateBatteryTelemetry(const std::vector<bms::BatteryUnitTelemetry>& newBatteryTelemetry,
                                const bms::AggregatedBatteryTelemetry& newAggregatedBatteryTelemetry);

private:
    std::unique_ptr<rest::Service> service;

    std::atomic_int latestBatteryTelemetryIndex{0};
    std::array<std::vector<bms::BatteryUnitTelemetry>, 2> latestBatteryTelemetry;
    std::array<bms::AggregatedBatteryTelemetry, 2> latestAggregatedBatteryTelemetry;

    void handleRequest(const std::vector<utility::string_t>& paths, web::http::http_request& message);
};



}