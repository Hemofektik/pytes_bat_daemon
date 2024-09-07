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

std::vector<BatteryUnitTelemetry> parseRawPowerTelemetry(const std::string& rawTelemetry) {
    std::vector<BatteryUnitTelemetry> data;
    std::istringstream rawStream{rawTelemetry};
    std::string line;

    // Skip the first empty and the header line
    std::getline(rawStream, line);
    std::cout << line << std::endl;
    std::getline(rawStream, line);
    std::cout << line << std::endl;

    while (std::getline(rawStream, line)) {
        std::istringstream iss(line);
        std::cout << line << std::endl;

        BatteryUnitTelemetry row;
        iss >> row.id >> row.volt_mV >> row.curr_mA >> row.tempr_mC >> row.tlow_mC >> row.thigh_mC
            >> row.vlow_mV >> row.vhigh_mV >> row.base_st >> row.volt_st >> row.curr_st >> row.temp_st
            >> row.coulomb_percent >> row.date >> row.time >> row.b_v_st >> row.b_t_st >> row.barcode >> row.devtype;

        data.push_back(row);
    }

    return data;
}


}