#pragma once
#include <CppLinuxSerial/SerialPort.hpp>

namespace pytes::bms
{

class SerialAdapter final
{

public:
    SerialAdapter();
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