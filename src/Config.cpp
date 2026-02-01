#include "Config.h"
#include <libconfig_chained.h>
#include <filesystem>


namespace pytes
{

namespace 
{

struct RawSerialAdapterConfig
{
    std::vector<std::string> devicePaths{"/dev/ttyUSB0"};
    int32_t baudRate{115200};
    int32_t numDataBits{8};
    std::string parity{"none"};
    int32_t numStopBits{1};
    bool hardwareFlowControlEnabled{false};
    bool softwareFlowControlEnabled{false};
};

mn::CppLinuxSerial::BaudRate selectBaudRate(int32_t baudRate)
{
    switch (baudRate)
    {
    case 0:
        return mn::CppLinuxSerial::BaudRate::B_0;
    case 50:
        return mn::CppLinuxSerial::BaudRate::B_50;
    case 75:
        return mn::CppLinuxSerial::BaudRate::B_75;
    case 110:
        return mn::CppLinuxSerial::BaudRate::B_110;
    case 134:
        return mn::CppLinuxSerial::BaudRate::B_134;
    case 150:
        return mn::CppLinuxSerial::BaudRate::B_150;
    case 200:
        return mn::CppLinuxSerial::BaudRate::B_200;
    case 300:
        return mn::CppLinuxSerial::BaudRate::B_300;
    case 600:
        return mn::CppLinuxSerial::BaudRate::B_600;
    case 1200:
        return mn::CppLinuxSerial::BaudRate::B_1200;
    case 1800:
        return mn::CppLinuxSerial::BaudRate::B_1800;
    case 2400:
        return mn::CppLinuxSerial::BaudRate::B_2400;
    case 4800:
        return mn::CppLinuxSerial::BaudRate::B_4800;
    case 9600:  
        return mn::CppLinuxSerial::BaudRate::B_9600;
    case 19200:
        return mn::CppLinuxSerial::BaudRate::B_19200;
    case 38400:
        return mn::CppLinuxSerial::BaudRate::B_38400;
    case 57600:
        return mn::CppLinuxSerial::BaudRate::B_57600;
    case 115200:
        return mn::CppLinuxSerial::BaudRate::B_115200;
    case 230400:
        return mn::CppLinuxSerial::BaudRate::B_230400;
    case 460800:
        return mn::CppLinuxSerial::BaudRate::B_460800;
    default:
        throw std::runtime_error("Invalid baud rate: " + std::to_string(baudRate));
    }
}

mn::CppLinuxSerial::NumDataBits selectNumDataBits(int32_t numDataBits)
{
    switch (numDataBits)
    {
    case 5:
        return mn::CppLinuxSerial::NumDataBits::FIVE;
    case 6:
        return mn::CppLinuxSerial::NumDataBits::SIX;
    case 7:
        return mn::CppLinuxSerial::NumDataBits::SEVEN;
    case 8:
        return mn::CppLinuxSerial::NumDataBits::EIGHT;
    default:
        throw std::runtime_error("Invalid number of data bits: " + std::to_string(numDataBits));
    }
}

mn::CppLinuxSerial::Parity selectParity(const std::string& parity)
{
    if (parity == "none")
    {
        return mn::CppLinuxSerial::Parity::NONE;
    }
    else if (parity == "odd")
    {
        return mn::CppLinuxSerial::Parity::ODD;
    }
    else if (parity == "even")
    {
        return mn::CppLinuxSerial::Parity::EVEN;
    }
    else
    {
        throw std::runtime_error("Invalid parity: " + parity);
    }
}

mn::CppLinuxSerial::NumStopBits selectNumStopBits(int32_t numStopBits)
{
    switch (numStopBits)
    {
    case 1:
        return mn::CppLinuxSerial::NumStopBits::ONE;
    case 2:
        return mn::CppLinuxSerial::NumStopBits::TWO;
    default:
        throw std::runtime_error("Invalid number of stop bits: " + std::to_string(numStopBits));
    }
}

bms::SerialAdapter::Config parseSerialAdapterConfig(const RawSerialAdapterConfig& serialAdapterConfig)
{
    bms::SerialAdapter::Config result {
        .devicePaths = serialAdapterConfig.devicePaths,
        .baudRate = selectBaudRate(serialAdapterConfig.baudRate),
        .numDataBits = selectNumDataBits(serialAdapterConfig.numDataBits),
        .parity = selectParity(serialAdapterConfig.parity),
        .numStopBits = selectNumStopBits(serialAdapterConfig.numStopBits),
        .hardwareFlowControl = serialAdapterConfig.hardwareFlowControlEnabled ? mn::CppLinuxSerial::HardwareFlowControl::ON : mn::CppLinuxSerial::HardwareFlowControl::OFF,
        .softwareFlowControl = serialAdapterConfig.softwareFlowControlEnabled ? mn::CppLinuxSerial::SoftwareFlowControl::ON : mn::CppLinuxSerial::SoftwareFlowControl::OFF
    };
    return result;
}

}

Config loadConfig(const std::string& configPath)
{
    Config result;
    libconfig::Config cfg;

    try
    {
        libconfig::Config cfg;
        cfg.readFile(configPath.c_str());

        libconfig::Config cfgSpec;
        cfgSpec.setOptions(libconfig::Config::OptionOpenBraceOnSeparateLine | libconfig::Config::OptionColonAssignmentForGroups | libconfig::Config::OptionColonAssignmentForNonGroups);
        cfgSpec.setTabWidth(4);

        std::stringstream errStream;
        libconfig::ChainedSetting cs(cfg.getRoot(), errStream);
        cs.captureExpectedSpecification(&cfgSpec);
        
        auto rest{cs["rest"]};
        std::string address = rest["address"].defaultValue("localhost");
        const int port{rest["port"].min(0).max(65535).defaultValue(7735).isMandatory()};

        auto serialAdapter = cs["serial_adapter"];
        auto devicePathsCfg = serialAdapter["device_paths"];
        if (!devicePathsCfg.exists()  || devicePathsCfg.getLength() == 0)
        {
            throw std::runtime_error("device_paths is not set");
        }

        std::vector<std::string> devicePaths;
        for(int i = 0; i < devicePathsCfg.getLength(); ++i) 
        {
            std::string const devicePath = devicePathsCfg[i].defaultValue("/dev/tty<ABC>").isMandatory();
            std::cout << devicePath << std::endl;
            devicePaths.push_back(devicePath);
        }

        RawSerialAdapterConfig serialAdapterConfig
        {
            .devicePaths = devicePaths,
            .baudRate = serialAdapter["baud_rate"].min(0).max(460800).defaultValue(115200).isMandatory(),
            .numDataBits = serialAdapter["num_data_bits"].min(5).max(8).defaultValue(8).isMandatory(),
            .parity = serialAdapter["parity"].defaultValue("none").isMandatory(),
            .numStopBits = serialAdapter["num_stop_bits"].min(1).max(2).defaultValue(1).isMandatory(),
            .hardwareFlowControlEnabled = serialAdapter["hardware_flow_control_enabled"].defaultValue(false).isMandatory(),
            .softwareFlowControlEnabled = serialAdapter["software_flow_control_enabled"].defaultValue(false).isMandatory()
        };

        const auto errStr{errStream.str()};
        if (cs.isAnyMandatorySettingMissing() || not errStr.empty())
        {
            auto tempDir{(std::filesystem::temp_directory_path() / "pytes_XXXXXX").string()};
            if (mkdtemp(tempDir.data()) == nullptr)
            {
                throw std::runtime_error{"Failed to create temporary directory with mkdtemp"};
            }
            auto cfgSpecStr = cs.getCapturedSpecification(tempDir + "/spec.cfg");
            std::stringstream err;
            err << "Cannot proceed until all mandatory settings are set within expected ranges." << std::endl;
            err << "Expected Config Layout: " << std::endl;
            err << "// -- begin --" << std::endl;
            err << cfgSpecStr;
            err << "// --- end ---" << std::endl << std::endl;
            err << "Encountered errors: " << errStr << std::endl; 
            throw std::runtime_error(err.str());
        }

        result.rest.address = address;
        result.rest.port = static_cast<uint16_t>(port);
        result.serialAdapter = parseSerialAdapterConfig(serialAdapterConfig);
    }
    catch(const libconfig::FileIOException& fioex)
    {
        throw std::runtime_error(std::string("I/O error while reading file: ") + configPath);
    }
    catch(const libconfig::ParseException& pex)
    {
        const auto err{std::string("Parse error at ") + pex.getFile() + ":" + std::to_string(pex.getLine()) + " - " + pex.getError()};
        throw std::runtime_error(err);
    }

    return result;
}

}