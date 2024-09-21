
#include <string_view>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>
#include "backward.hpp"

#include <tuple>

#include <bms/Telemetry.h>


using namespace pytes;


namespace Catch 
{
template <>
struct StringMaker<std::optional<int>>
{
    static std::string convert(std::optional<int> const &value)
    {
        if (value)
            return std::to_string(*value);
        else
            return std::string("<empty>");
    }
};

template <>
struct StringMaker<std::optional<std::string>>
{
    static std::string convert(std::optional<std::string> const &value)
    {
        if (value)
            return *value;
        else
            return std::string("<empty>");
    }
};

template <>
struct StringMaker<std::nullopt_t>
{
    static std::string convert(std::nullopt_t const &)
    {
        return std::string("<empty>");
    }
};

}

std::tuple<std::vector<bms::BatteryUnitTelemetry>, bms::AggregatedBatteryTelemetry> testCase(
    const std::vector<bms::BatteryUnitTelemetry>& inputState, 
    const bms::AggregatedBatteryTelemetry& expectedOutput)
{
	return std::tuple<std::vector<bms::BatteryUnitTelemetry>, bms::AggregatedBatteryTelemetry>(inputState, expectedOutput);
}

SCENARIO( "BMS Battery Telemetry can be aggregated", "[bms::telemetry]" ) 
{
    backward::SignalHandling sh;

    SECTION("Aggregate")
	{
        const std::vector<bms::BatteryUnitTelemetry> emptyTelemetry{};
        const std::vector<bms::BatteryUnitTelemetry> singleEntryTelemetry{
            {0, 52566, -2763, 30000, 29000, 29000, 3283, 3288, bms::BatteryState::Discharging, "Normal", "Normal", "Normal", 57, "2024-09-06", "09:42:04", "Normal", "Normal", "ES1000899P021108", "E-BOX-48100R-C"},
        };

        const bms::AggregatedBatteryTelemetry emptyAggregatedTelemetry{};
        const bms::AggregatedBatteryTelemetry singeEntryAggregatedTelemetry{};

        std::vector<bms::BatteryUnitTelemetry> telemetry;
        bms::AggregatedBatteryTelemetry expectedAggregatedTelemetry;
        std::tie(telemetry, expectedAggregatedTelemetry) = GENERATE_COPY(table<std::vector<bms::BatteryUnitTelemetry>, bms::AggregatedBatteryTelemetry>({
            testCase(emptyTelemetry, emptyAggregatedTelemetry),
            testCase(singleEntryTelemetry, singeEntryAggregatedTelemetry),
        }));

        auto aggregatedTelemetry{bms::aggregateBatteryTelemetry(telemetry)};
        CAPTURE(telemetry);            
        CHECK(aggregatedTelemetry.avgCoulomb_percent == expectedAggregatedTelemetry.avgCoulomb_percent);
        CHECK(aggregatedTelemetry.avgCurr_mA == expectedAggregatedTelemetry.avgCurr_mA);
        CHECK(aggregatedTelemetry.avgTempr_mC == expectedAggregatedTelemetry.avgTempr_mC);
        CHECK(aggregatedTelemetry.avgVolt_mV == expectedAggregatedTelemetry.avgVolt_mV);
        CHECK(aggregatedTelemetry.baseState == expectedAggregatedTelemetry.baseState);
        CHECK(aggregatedTelemetry.date == expectedAggregatedTelemetry.date);
        CHECK(aggregatedTelemetry.devtype == expectedAggregatedTelemetry.devtype);
        CHECK(aggregatedTelemetry.maxVoltHigh_mV == expectedAggregatedTelemetry.maxVoltHigh_mV);
        CHECK(aggregatedTelemetry.minVoltLow_mV == expectedAggregatedTelemetry.minVoltLow_mV);
        CHECK(aggregatedTelemetry.time == expectedAggregatedTelemetry.time);
    }
}
