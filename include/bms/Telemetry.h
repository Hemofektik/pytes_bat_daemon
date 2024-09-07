#pragma once

#include <vector>
#include <optional>
#include <string>

namespace pytes::bms
{

struct BatteryUnitTelemetry {
    int32_t id;
    std::optional<int32_t> volt_mV;
    std::optional<int32_t> curr_mA;
    std::optional<int32_t> tempr_mC;
    std::optional<int32_t> tlow_mC;
    std::optional<int32_t> thigh_mC;
    std::optional<int32_t> vlow_mV;
    std::optional<int32_t> vhigh_mV;
    std::string base_st;
    std::string volt_st;
    std::string curr_st;
    std::string temp_st;
    std::optional<int32_t> coulomb_percent;
    std::string date;
    std::string time;
    std::string b_v_st;
    std::string b_t_st;
    std::string barcode;
    std::string devtype;
};

std::vector<BatteryUnitTelemetry> parseRawPowerTelemetry(const std::string& rawTelemetry);

}