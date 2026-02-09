#include <bms/SerialAdapter.h>
#include <thread>
#include <chrono>

using namespace std::chrono_literals;
using namespace mn::CppLinuxSerial;

namespace pytes::bms
{


SerialAdapter::SerialAdapter(const Config& config)
: serialPort("", config.baudRate, config.numDataBits, config.parity, config.numStopBits, config.hardwareFlowControl, config.softwareFlowControl)
{
    std::string const sentinel{"PYTES>"};
    bool found = false;
    for(const auto& path : config.devicePaths) 
    {
        std::cout << "Checking device: "  << path << std::endl;

        try 
        {
            serialPort.SetDevice(path);
            serialPort.SetTimeout(100); // Block for up to 100ms to receive data
            serialPort.Open();

            if(serialPort.GetState() != mn::CppLinuxSerial::State::OPEN) 
            {
                std::cout << "Cannot open device: "  << path << std::endl;
                continue;
            }

            serialPort.Write("\n"); // request default answer
            std::this_thread::sleep_for(200ms);
            std::string readData;
            auto const timeout{1000ms};
            auto const stepDuration{100ms};
            auto waitedFor{0ms};
            while(waitedFor < timeout) 
            {
                if(serialPort.Available()) 
                {
                    serialPort.Read(readData);
                    if(readData.find(sentinel) != std::string::npos) 
                    {
                        found = true;
                        activeDevicePath = path;
                        break;
                    }
                }
                std::this_thread::sleep_for(stepDuration);
                waitedFor += stepDuration;
            }
            serialPort.Close();
            if(found) 
            {
                std::cout << "Found PYTES device at: "  << path << std::endl;
                serialPort.SetDevice(path);
                serialPort.SetTimeout(5000);
                serialPort.Open();
                break;
            }
        }
        catch(const mn::CppLinuxSerial::Exception&) 
        {
            // Try next device
            try { serialPort.Close(); } catch(...) {}
        }
    }
    if(!found) 
    {
        throw std::runtime_error("No serial device responded with the expected sentinel: " + sentinel);
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
    int const maxRetries{3};

    for(int retry = 0; retry < maxRetries; ++retry)
    {
        try
        {
            serialPort.Write("\npwr\n");
            std::this_thread::sleep_for(200ms);

            std::string totalReadData;
            bool foundStartToken = false;
            auto startTime = std::chrono::steady_clock::now();
            auto const maxDurationAfterStart{5000ms};
            
            // Read until we find both start and end tokens
            while(true)
            {
                std::string readData;
                serialPort.Read(readData);
                
                if(readData.empty())
                {
                    // Timeout occurred (Read returns empty on timeout)
                    break;
                }
                
                totalReadData.append(readData);
                
                // Check if we found the start token
                if(!foundStartToken && totalReadData.find_last_of(messageStartToken) != std::string::npos)
                {
                    foundStartToken = true;
                    startTime = std::chrono::steady_clock::now();
                }
                
                // If we found the start token, check for end token
                if(foundStartToken)
                {
                    if(totalReadData.rfind(messageEndToken) != std::string::npos)
                    {
                        // Found both tokens, extract and return
                        auto const start{totalReadData.find_last_of(messageStartToken)};
                        auto const end{totalReadData.rfind(messageEndToken)};
                        
                        return totalReadData.substr(start + 1, end - start - 1);
                    }
                    
                    // Check if we've exceeded the time limit after finding start token
                    auto elapsed = std::chrono::steady_clock::now() - startTime;
                    if(elapsed >= maxDurationAfterStart)
                    {
                        break;
                    }
                }
            }
        }
        catch(const mn::CppLinuxSerial::Exception& e)
        {
            throw std::runtime_error(std::string("Serial Device Error: ") + e.what());
        }

        // If we get here, timeout occurred; continue to retry unless it's the last attempt
        if(retry == maxRetries - 1)
        {
            throw std::runtime_error("Timeout occured! Did not receive data from the serial device after " + std::to_string(maxRetries) + " attempts.");
        }
    }

    throw std::runtime_error("Timeout occured! Did not receive data from the serial device.");
}

}