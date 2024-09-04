#include <CppLinuxSerial/SerialPort.hpp>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using namespace mn::CppLinuxSerial;

int main() 
{
    std::string const sentinel{"PYTES>"};
    auto const timeout{5000ms};
    auto const stepDuration{100ms};
    auto waitedFor{0ms};

	SerialPort serialPort("/dev/ttyUSB0", BaudRate::B_115200, NumDataBits::EIGHT, Parity::NONE, NumStopBits::ONE);
	serialPort.Open();

	serialPort.Write("pwr\n");

    while(serialPort.Available() == 0)
    {
        std::this_thread::sleep_for(stepDuration);
        waitedFor += stepDuration;
    }

    while(serialPort.Available() || waitedFor < timeout)
    {
	    std::string readData;
        serialPort.Read(readData);

        if(readData.rfind(sentinel) == std::string::npos)
        {
            std::this_thread::sleep_for(stepDuration);
            waitedFor += stepDuration;
        }
        else
        {
            std::cout << readData << std::endl;
            break;
        }
    }

	serialPort.Close();
}
