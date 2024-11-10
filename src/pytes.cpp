#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <chrono>
#include <thread>
#include "backward.hpp"

#include <bms/SerialAdapter.h>
#include <bms/Telemetry.h>
#include "RestService.h"
#include "Config.h"

using namespace std::chrono_literals;
using namespace pytes;

auto to_string(int32_t x) -> std::string { return std::to_string(x); };

std::ostream& operator<<(std::ostream& os, const std::optional<int32_t>& opt)
{
    os << opt.transform( to_string ).value_or("-");
    return os;
}


std::ostream& operator<<(std::ostream& os, const std::optional<std::string>& opt)
{
    os << opt.value_or("-");
    return os;
}

std::ostream& operator<<(std::ostream& os, const bms::BatteryState& batState)
{
    switch (batState)
    {
    case bms::BatteryState::Unknown:
        os << "Unknown";
        break;
    case bms::BatteryState::Absent:
        os << "Absent";
        break;
    case bms::BatteryState::Charging:
        os << "Charge";
        break;
    case bms::BatteryState::Discharging:
        os << "Dischg";
        break;
    case bms::BatteryState::Idle:
        os << "Idle";
        break;
    }

    return os;
}

std::tm localTime(const std::chrono::system_clock::time_point& timestamp)
{
    const auto time{std::chrono::system_clock::to_time_t(timestamp)};
    const auto localTime{std::localtime(&time)};
    if(localTime == nullptr)
    {
        throw std::runtime_error("Failed to convert time to local time");
    }
    return *localTime;
}

int main()
{
    backward::SignalHandling sh;

    const auto config{loadConfig("pytes.cfg")};

    std::optional<bms::SerialAdapter> bmsAdapter{};
    std::optional<RestService> restService;
    bmsAdapter.emplace();

    try
    {
        restService.emplace(config.rest);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Unable to create REST service: "  << e.what() << std::endl;
        return 1;
    }

    const bool debugLogEnabled{true};
    bool checkForChargingResetAfterMidnight{false};
    bms::AccumulatedBatteryTelemetry accumulatedBatteryTelemetry{};
    accumulatedBatteryTelemetry.lastUpdateTime = std::chrono::system_clock::now();
    bms::BatteryState previousAggregatedBaseState{bms::BatteryState::Unknown};

    while(true)
    {
        std::this_thread::sleep_for(100ms);

        try
        {
            auto const rawPowerTelemetry{bmsAdapter->readRawPowerTelemetry()};
            auto const parsedPowerTelemetry{bms::parseRawPowerTelemetry(rawPowerTelemetry)};

            std::vector<bms::BatteryUnitTelemetry> newBatteryTelemetry;
            for (const auto& row : parsedPowerTelemetry)
            {    
                if(row.base_state == bms::BatteryState::Absent)
                {
                    continue;
                }

                if(row.base_state == bms::BatteryState::Unknown)
                {
                    std::cerr << "Encountered unexpected battery telemetry:" << std::endl << rawPowerTelemetry << std::endl;
                    continue;
                }

                newBatteryTelemetry.push_back(row);

                if(debugLogEnabled)
                {
                    std::cout << row.id << " " << row.volt_mV << " " << row.curr_mA << " "
                            << row.tempr_mC << " " << row.tlow_mC << " " << row.thigh_mC << " " 
                            << row.vlow_mV << " " << row.vhigh_mV << " " << row.base_state << " " 
                            << row.volt_st << " " << row.curr_st << " " << row.temp_st << " " 
                            << row.coulomb_percent << " " << row.date << "T" << row.time << " " 
                            << row.b_v_st << " " << row.b_t_st << " " 
                            << row.barcode << " " << row.devtype << std::endl;
                }
            }


            auto const newAggregatedTelemetry{bms::aggregateBatteryTelemetry(parsedPowerTelemetry)};

            const auto now{std::chrono::system_clock::now()};
            const auto localLastUpdateTime{localTime(accumulatedBatteryTelemetry.lastUpdateTime)};
            const auto localCurrentUpdateTime{localTime(now)};
            const bool passedMidnight{localCurrentUpdateTime.tm_hour < localLastUpdateTime.tm_hour};
            if(passedMidnight)
            {
                checkForChargingResetAfterMidnight = true;
            }

            accumulateBatteryTelemetry(newAggregatedTelemetry, now, accumulatedBatteryTelemetry);
            bool const switchedToCharging{newAggregatedTelemetry.baseState == bms::BatteryState::Charging && previousAggregatedBaseState != bms::BatteryState::Charging};

            // Reset accumulatedBatteryTelemetry after midnight when first charging of day starts (new cycle)
            if (checkForChargingResetAfterMidnight && switchedToCharging)
            {
                accumulatedBatteryTelemetry.energyCharged_kWh = 0.0;
                accumulatedBatteryTelemetry.energyDischarged_kWh = 0.0;
                checkForChargingResetAfterMidnight = false;
            }
            previousAggregatedBaseState = newAggregatedTelemetry.baseState;

            restService->updateBatteryTelemetry(newBatteryTelemetry, newAggregatedTelemetry, accumulatedBatteryTelemetry);

        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl << "Reconnecting to BMS adapter" << std::endl;
            bmsAdapter.emplace();
        }
    }
}
