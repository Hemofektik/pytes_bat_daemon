#pragma once
#include <CppLinuxSerial/SerialPort.hpp>

namespace pytes::bms
{

class SerialAdapter final
{
public:
    struct Config
    {
        std::string devicePath{"/dev/ttyUSB0"};
        mn::CppLinuxSerial::BaudRate baudRate{mn::CppLinuxSerial::BaudRate::B_115200};
        mn::CppLinuxSerial::NumDataBits numDataBits{mn::CppLinuxSerial::NumDataBits::EIGHT};
        mn::CppLinuxSerial::Parity parity{mn::CppLinuxSerial::Parity::NONE};
        mn::CppLinuxSerial::NumStopBits numStopBits{mn::CppLinuxSerial::NumStopBits::ONE};
        mn::CppLinuxSerial::HardwareFlowControl hardwareFlowControl{mn::CppLinuxSerial::HardwareFlowControl::OFF};
        mn::CppLinuxSerial::SoftwareFlowControl softwareFlowControl{mn::CppLinuxSerial::SoftwareFlowControl::OFF};
    };

    SerialAdapter(const Config& config);
    ~SerialAdapter();

	SerialAdapter(SerialAdapter const &) = delete;
	SerialAdapter &operator=(SerialAdapter const &) = delete;
	SerialAdapter(SerialAdapter &&) = delete;
	SerialAdapter &operator=(SerialAdapter &&) = delete;
    
    std::string readRawPowerTelemetry();

private:
    mn::CppLinuxSerial::SerialPort serialPort;
};


}