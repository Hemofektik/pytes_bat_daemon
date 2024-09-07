#include <sstream>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <chrono>
#include <thread>

#include <bms/SerialAdapter.h>
#include <bms/Telemetry.h>

using namespace std::chrono_literals;
using namespace pytes;

auto to_string(int32_t x) -> std::string { return std::to_string(x); };

std::ostream& operator<<(std::ostream& os, const std::optional<int32_t>& opt)
{
    os << opt.transform( to_string ).value_or("-");
    return os;
}


std::ostream& operator<<(std::ostream& os, const bms::BatteryState& batState)
{
    os << "Unknown";
    return os;
}

int main()
{
    auto bmsAdapter{bms::SerialAdapter()};

    while(true)
    {
        std::this_thread::sleep_for(100ms);

        try
        {
            auto const rawPowerTelemetry{bmsAdapter.readRawPowerTelemetry()};
            auto const parsedPowerTelemetry{bms::parseRawPowerTelemetry(rawPowerTelemetry)};

            
            std::cout << rawPowerTelemetry << std::endl;

            // Print parsed data for verification
            for (const auto& row : parsedPowerTelemetry) {
                std::cout << row.id << " " << row.volt_mV << " " << row.curr_mA << " "
                        << row.tempr_mC << " " << row.tlow_mC << " " << row.thigh_mC << " " << row.vlow_mV << " "
                        << row.vhigh_mV << " " << row.base_state << " " << row.volt_st << " " << row.curr_st << " "
                        << row.temp_st << " " << row.coulomb_percent << " " << row.date << "T" << row.time << " " << row.b_v_st << " "
                        << row.b_t_st << " " << row.barcode << " " << row.devtype << std::endl;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}
