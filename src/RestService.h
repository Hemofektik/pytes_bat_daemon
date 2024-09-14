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

    void updateBatteryTelemetry(const std::vector<bms::BatteryUnitTelemetry>& newBatteryTelemetry);

private:
    std::unique_ptr<rest::Service> service;

    std::atomic_int latestBatteryTelemetryIndex{0};
    std::array<std::vector<bms::BatteryUnitTelemetry>, 2> latestBatteryTelemetry;
};



}