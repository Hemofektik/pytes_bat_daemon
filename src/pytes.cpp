#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <chrono>
#include <thread>

#include <bms/SerialAdapter.h>
#include <bms/Telemetry.h>
#include "RestService.h"

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

int main()
{
    std::vector<bms::BatteryUnitTelemetry> mostRecentBatteryTelemetry;
    std::optional<bms::SerialAdapter> bmsAdapter{};
    bmsAdapter.emplace();

    try
    {
        RestService restService{};
    }
    catch(const std::exception& e)
    {
        std::cerr << "Unable to create REST service: "  << e.what() << std::endl;
        return 1;
    }

    bool const debugLogEnabled{true};

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
            
            std::swap(mostRecentBatteryTelemetry, newBatteryTelemetry);
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << std::endl << "Reconnecting to BMS adapter" << std::endl;
            bmsAdapter.emplace();
        }
    }
}
