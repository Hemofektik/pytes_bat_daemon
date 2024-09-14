#pragma once

#include <vector>
#include <optional>
#include <string>

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
    int32_t id;
    std::optional<int32_t> volt_mV;
    std::optional<int32_t> curr_mA;
    std::optional<int32_t> tempr_mC;
    std::optional<int32_t> tlow_mC;
    std::optional<int32_t> thigh_mC;
    std::optional<int32_t> vlow_mV;
    std::optional<int32_t> vhigh_mV;
    BatteryState base_state;
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

std::vector<BatteryUnitTelemetry> parseRawPowerTelemetry(const std::string& rawTelemetry);

}