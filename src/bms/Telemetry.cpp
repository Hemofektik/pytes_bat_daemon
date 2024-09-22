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

AggregatedBatteryTelemetry aggregateBatteryTelemetry(const std::vector<BatteryUnitTelemetry>& batteryTelemetry)
{
    if(batteryTelemetry.empty())
    {
        return AggregatedBatteryTelemetry{};
    }

    auto present{[](const auto& row){ return row.base_state != BatteryState::Absent; }};
    auto presentBatteryTelemetry{batteryTelemetry | std::views::filter(present) | std::views::common};

    if(std::ranges::empty(presentBatteryTelemetry))
    {
        AggregatedBatteryTelemetry aggregatedData{};
        aggregatedData.baseState = BatteryState::Absent;
        return aggregatedData;
    }

    auto countState{[](std::map<BatteryState, int> stateCount, BatteryUnitTelemetry const &b)
    {
        stateCount[b.base_state]++;
        return stateCount;
    }};

    std::map<BatteryState, int> stateCount{std::accumulate(presentBatteryTelemetry.begin(), presentBatteryTelemetry.end(),
                                    std::map<BatteryState, int>{},
                                    countState)};

    std::vector<std::pair<BatteryState, int>> stateCountVec;
    std::copy(stateCount.begin(), stateCount.end(), std::back_inserter(stateCountVec));

    std::ranges::sort(stateCountVec, {}, &std::pair<BatteryState, int>::second);
 
    auto computeAverage{[](auto range, auto count, auto value)
    {
        const auto numElements{std::ranges::count_if(range, count)};
        const auto sum{std::transform_reduce(range.begin(), range.end(), 0, std::plus<int64_t>(), value)};
        return std::optional<int32_t>{sum / numElements};
    }};

    const int64_t totalPower_microW{std::transform_reduce(presentBatteryTelemetry.begin(), presentBatteryTelemetry.end(), 0, std::plus<int64_t>(),
                                    [](const BatteryUnitTelemetry& element) { return element.volt_mV.value_or(0) * element.curr_mA.value_or(0); } )};

    AggregatedBatteryTelemetry aggregatedData
    {
        .totalPower_W = totalPower_microW / 1'000'000,
        .avgVolt_mV = computeAverage(presentBatteryTelemetry, [](const BatteryUnitTelemetry& element) { return element.volt_mV.has_value(); }, [](const BatteryUnitTelemetry& element) { return element.volt_mV.value_or(0); }),
        .avgCurr_mA = computeAverage(presentBatteryTelemetry, [](const BatteryUnitTelemetry& element) { return element.curr_mA.has_value(); }, [](const BatteryUnitTelemetry& element) { return element.curr_mA.value_or(0); }),
        .avgTempr_mC = computeAverage(presentBatteryTelemetry, [](const BatteryUnitTelemetry& element) { return element.tempr_mC.has_value(); }, [](const BatteryUnitTelemetry& element) { return element.tempr_mC.value_or(0); }),
        
        .minVoltLow_mV = std::ranges::min_element(presentBatteryTelemetry, {}, &BatteryUnitTelemetry::vlow_mV)->vlow_mV,
        .maxVoltHigh_mV = std::ranges::max_element(presentBatteryTelemetry, {}, &BatteryUnitTelemetry::vhigh_mV)->vhigh_mV,
        .baseState = stateCountVec.back().first,

        .avgCoulomb_percent = computeAverage(presentBatteryTelemetry, [](const BatteryUnitTelemetry& element) { return element.coulomb_percent.has_value(); }, [](const BatteryUnitTelemetry& element) { return element.coulomb_percent.value_or(0); }),

        .date = std::ranges::min_element(presentBatteryTelemetry, {}, &BatteryUnitTelemetry::date)->date,
        .time = std::ranges::min_element(presentBatteryTelemetry, {}, &BatteryUnitTelemetry::time)->time,
        .devtype = batteryTelemetry.front().devtype, // TODO: return all device types found with count
    };

    return aggregatedData;
}


}