#include "Config.h"
#include <libconfig_chained.h>
#include <filesystem>


namespace pytes
{

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

        const int port{cs["rest"]["port"].min(0).max(65535).defaultValue(7735).isMandatory()};

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

        result.rest.port = static_cast<uint16_t>(port);
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