#include <CppLinuxSerial/SerialPort.hpp>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using namespace mn::CppLinuxSerial;

std::string readRawPytesOutput()
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

        while(serialPort.Available() == 0)
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


int main() 
{
    std::cout << readRawPytesOutput() << std::endl;
}
