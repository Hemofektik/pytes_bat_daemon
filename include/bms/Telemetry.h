#pragma once

#include <vector>
#include <optional>
#include <string>
#include <chrono>

namespace pytes::bms
{

enum class BatteryState
{
    Unknown,
    Absent,
    Charging,
    Discharging,
    Idle
};

struct BatteryUnitTelemetry {
    int32_t id{0};
    std::optional<int32_t> volt_mV;
    std::optional<int32_t> curr_mA;
    std::optional<int32_t> tempr_mC;
    std::optional<int32_t> tlow_mC;
    std::optional<int32_t> thigh_mC;
    std::optional<int32_t> vlow_mV;
    std::optional<int32_t> vhigh_mV;
    BatteryState base_state{BatteryState::Unknown};
    std::optional<std::string> volt_st;
    std::optional<std::string> curr_st;
    std::optional<std::string> temp_st;
    std::optional<int32_t> coulomb_percent;
    std::optional<std::string> date;
    std::optional<std::string> time;
    std::optional<std::string> b_v_st;
    std::optional<std::string> b_t_st;
    std::optional<std::string> barcode;
    std::optional<std::string> devtype;
};

struct AggregatedBatteryTelemetry {
    std::optional<float> totalPower_W;
    std::optional<float> totalCurrent_A;
    std::optional<float> totalEnergy_kWh;
    std::optional<int32_t> avgVolt_mV;
    std::optional<int32_t> avgCurr_mA;
    std::optional<int32_t> avgTempr_mC;
    std::optional<int32_t> minVoltLow_mV;
    std::optional<int32_t> maxVoltHigh_mV;
    BatteryState baseState{BatteryState::Unknown};
    std::optional<float> avgCoulomb_percent;
    std::optional<std::string> date;
    std::optional<std::string> time;
    std::optional<std::string> devtype;
};


struct AccumulatedBatteryTelemetry {
    double energyCharged_kWh{0.0};
    double energyDischarged_kWh{0.0};
    std::chrono::time_point<std::chrono::system_clock> lastUpdateTime{};
};


std::vector<BatteryUnitTelemetry> parseRawPowerTelemetry(const std::string& rawTelemetry);

AggregatedBatteryTelemetry aggregateBatteryTelemetry(const std::vector<BatteryUnitTelemetry>& batteryTelemetry);
void accumulateBatteryTelemetry(const AggregatedBatteryTelemetry& aggregatedBatteryTelemetry, std::chrono::time_point<std::chrono::system_clock> aggregatedBatteryTelemetryTimestamp, AccumulatedBatteryTelemetry& accumulatedBatteryTelemetry);

}