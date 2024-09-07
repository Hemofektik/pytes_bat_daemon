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

std::string readRawTelemetry()
{
    std::string const sentinel{"PYTES>"};
    char const messageStartToken{'@'};
    std::string_view const messageEndToken{"Command completed successfully"};
    auto const timeout{5000ms};
    auto const stepDuration{100ms};
    auto waitedFor{0ms};

    try
    {
        SerialPort serialPort("/dev/ttyUSB0", BaudRate::B_115200, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
        serialPort.Open();

        serialPort.Write("pwr\n");

        while(serialPort.Available() == 0 && waitedFor < timeout)
        {
            std::this_thread::sleep_for(stepDuration);
            waitedFor += stepDuration;
        }

        std::string totalReadData;
        while(serialPort.Available() || waitedFor < timeout)
        {
            std::string readData;
            serialPort.Read(readData);
            totalReadData.append(readData);

            if(readData.rfind(sentinel) == std::string::npos)
            {
                std::this_thread::sleep_for(stepDuration);
                waitedFor += stepDuration;
            }
            else
            {
                auto const start{totalReadData.find_last_of(messageStartToken)};
                auto const end{totalReadData.rfind(messageEndToken)};
                if(start == std::string::npos || end == std::string::npos)
                {
                    throw std::runtime_error("Received unexpected output format: " + totalReadData);
                }

                return totalReadData.substr(start + 1, end - start - 1);
            }
        }

        serialPort.Close();
    }
    catch(const mn::CppLinuxSerial::Exception& e)
    {
        throw std::runtime_error(std::string("Serial Device Error: ") + e.what());
    }

    throw std::runtime_error("Timeout occured! Did not read receive data from the serial device.");
}

std::vector<BatteryUnitTelemetry> parseRawTelemetry(const std::string& rawTelemetry) {
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