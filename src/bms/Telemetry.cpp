#include <bms/Telemetry.h>
#include <CppLinuxSerial/SerialPort.hpp>
#include "OptionalValue.h"
#include <sstream>
#include <thread>
#include <chrono>

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


}