#pragma once
#include <rest/Service.h>
#include <bms/Telemetry.h>
#include <atomic>

namespace pytes
{

class RestService
{
public:
    struct Config
    {
        std::string address{};
        uint16_t port{};
    };

    static Config loadConfig(const std::string& configPath);

    RestService(const Config& config);
    ~RestService();

    void updateBatteryTelemetry(const std::vector<bms::BatteryUnitTelemetry>& newBatteryTelemetry,
                                const bms::AggregatedBatteryTelemetry& newAggregatedBatteryTelemetry,
                                const bms::AccumulatedBatteryTelemetry& newDailyAccumulatedBatteryTelemetry);

private:
    std::unique_ptr<rest::Service> service;

    std::atomic_int latestBatteryTelemetryIndex{0};
    std::array<std::vector<bms::BatteryUnitTelemetry>, 2> latestBatteryTelemetry;
    std::array<bms::AggregatedBatteryTelemetry, 2> latestAggregatedBatteryTelemetry;
    std::array<bms::AccumulatedBatteryTelemetry, 2> latestDailyAccumulatedBatteryTelemetry;

    void handleRequest(const std::vector<utility::string_t>& paths, web::http::http_request& message);
};



}