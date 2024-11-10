#pragma once
#include "RestService.h"

namespace pytes
{

struct Config
{
    RestService::Config rest;
};

Config loadConfig(const std::string& configPath);

}