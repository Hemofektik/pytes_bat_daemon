#include <bms/Telemetry.h>
#include <CppLinuxSerial/SerialPort.hpp>
#include "OptionalValue.h"
#include <sstream>
#include <thread>
#include <chrono>
#include <ranges>
#include <map>
#include <numeric>
#include <algorithm>

using namespace std::chrono_literals;
using namespace mn::CppLinuxSerial;

namespace pytes::bms
{


std::istream& operator>> (std::istream& is, BatteryState& batState)
{
    std::string str;
    is >> str;
    if(str == "Absent")
    {
        batState = BatteryState::Absent;
    }
    else if(str == "Charge")
    {
        batState = BatteryState::Charging;
    }
    else if(str == "Dischg")
    {
        batState = BatteryState::Discharging;
    }
    else if(str == "Idle")
    {
        batState = BatteryState::Idle;
    }
    else
    {
        batState = BatteryState::Unknown;
    }

    return is;
}

std::vector<BatteryUnitTelemetry> parseRawPowerTelemetry(const std::string& rawTelemetry) 
{
    std::vector<BatteryUnitTelemetry> data;
    std::istringstream rawStream{rawTelemetry};
    std::string line;

    // Skip the first empty and the header line
    std::getline(rawStream, line);
    std::getline(rawStream, line);

    while (std::getline(rawStream, line)) 
    {
        if(line.size() < 162)
        {
            continue;
        }

        std::istringstream iss(line);

        BatteryUnitTelemetry row;
        iss >> row.id >> row.volt_mV >> row.curr_mA 
            >> row.tempr_mC >> row.tlow_mC >> row.thigh_mC
            >> row.vlow_mV >> row.vhigh_mV >> row.base_state 
            >> row.volt_st >> row.curr_st >> row.temp_st
            >> row.coulomb_percent >> row.date >> row.time 
            >> row.b_v_st >> row.b_t_st >> row.barcode >> row.devtype;

        data.push_back(row);
    }

    return data;
}

AggregatedBatteryTelemetry aggregateBatteryTelemetry(const  std::vector<BatteryUnitTelemetry>& batteryTelemetry)
{
    int32_t avgVolt_mV{0};
    int32_t avgCurr_mA{0};
    int32_t avgTempr_mC{0};
    int32_t minVoltLow_mV{INT32_MAX};
    int32_t maxVoltHigh_mV{0};
    int32_t avgCoulomb_percent{0};

    auto countState = [](std::map<BatteryState, int> stateCount, BatteryUnitTelemetry const &b)
    {
        stateCount[b.base_state]++;
        return stateCount;
    };

    auto present{[](const auto& row){ return row.base_state != BatteryState::Absent; }};
    auto presentBatteryTelemetry{batteryTelemetry | std::views::filter(present) | std::views::common};

    std::map<BatteryState, int> stateCount = std::accumulate(std::next(presentBatteryTelemetry.begin()), presentBatteryTelemetry.end(),
                                    std::map<BatteryState, int>{},
                                    countState);

    std::vector<std::pair<BatteryState, int>> stateCountVec;
    std::copy(stateCount.begin(), stateCount.end(), std::back_inserter(stateCountVec));

    auto sortedCount{std::ranges::sort(stateCountVec, {}, &std::pair<BatteryState, int>::second)};

    for (const auto& row : presentBatteryTelemetry)
    {
        if(row.volt_mV.has_value())
        {
            avgVolt_mV += row.volt_mV.value();
        }

        if(row.curr_mA.has_value())
        {
            avgCurr_mA += row.curr_mA.value();
        }

        if(row.tempr_mC.has_value())
        {
            avgTempr_mC += row.tempr_mC.value();
        }

        if(row.vlow_mV.has_value())
        {
            minVoltLow_mV = std::min(minVoltLow_mV, row.vlow_mV.value());
        }

        if(row.vhigh_mV.has_value())
        {
            maxVoltHigh_mV = std::max(maxVoltHigh_mV, row.vhigh_mV.value());
        }

        if(row.coulomb_percent.has_value())
        {
            avgCoulomb_percent += row.coulomb_percent.value();
        }
    }

    AggregatedBatteryTelemetry aggregatedData{};
    if(!batteryTelemetry.empty())
    {
        aggregatedData.avgVolt_mV = avgVolt_mV / batteryTelemetry.size();
        aggregatedData.avgCurr_mA = avgCurr_mA / batteryTelemetry.size();
        aggregatedData.avgTempr_mC = avgTempr_mC / batteryTelemetry.size();
        aggregatedData.minVoltLow_mV = minVoltLow_mV;
        aggregatedData.maxVoltHigh_mV = maxVoltHigh_mV;
        aggregatedData.avgCoulomb_percent = avgCoulomb_percent / batteryTelemetry.size();
        aggregatedData.baseState = batteryTelemetry.front().base_state;
        aggregatedData.date = batteryTelemetry.front().date;
        aggregatedData.time = batteryTelemetry.front().time;
        aggregatedData.devtype = batteryTelemetry.front().devtype;
    }

    return aggregatedData;
}


}