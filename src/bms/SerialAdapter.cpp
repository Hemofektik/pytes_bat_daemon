#include <bms/SerialAdapter.h>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using namespace mn::CppLinuxSerial;

namespace pytes::bms
{

SerialAdapter::SerialAdapter() 
: serialPort("/dev/ttyUSB0", BaudRate::B_115200, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE)
{
    try
    {
        serialPort.Open();
    }
    catch(const mn::CppLinuxSerial::Exception& e)
    {
        throw std::runtime_error(std::string("Serial Device Error: ") + e.what());
    }
}

SerialAdapter::~SerialAdapter()
{
    try
    {
        serialPort.Close();
    }
    catch(const mn::CppLinuxSerial::Exception& e)
    {
        // ignore errors here since we are done with the serial port
    }
}

std::string SerialAdapter::readRawPowerTelemetry()
{
    std::string const sentinel{"PYTES>"};
    char const messageStartToken{'@'};
    std::string_view const messageEndToken{"Command completed successfully"};
    auto const timeout{5000ms};
    auto const stepDuration{100ms};
    auto waitedFor{0ms};

    try
    {
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
            if(serialPort.Available())
            {
                serialPort.Read(readData);
                totalReadData.append(readData);
            }

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
    }
    catch(const mn::CppLinuxSerial::Exception& e)
    {
        throw std::runtime_error(std::string("Serial Device Error: ") + e.what());
    }

    throw std::runtime_error("Timeout occured! Did not read receive data from the serial device.");
}

}