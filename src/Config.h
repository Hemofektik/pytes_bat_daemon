#pragma once
#include "RestService.h"
#include "bms/SerialAdapter.h"

namespace pytes
{

struct Config
{
    RestService::Config rest;
    bms::SerialAdapter::Config serialAdapter;
};

Config loadConfig(const std::string& configPath);

}