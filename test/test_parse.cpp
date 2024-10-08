
#include <string_view>
#include <catch2/catch_test_macros.hpp>

#include <bms/Telemetry.h>

const std::string_view bmsOutput = R"(@
Power Volt   Curr   Tempr  Tlow   Thigh  Vlow   Vhigh  Base.St  Volt.St  Curr.St  Temp.St  Coulomb  Time                 B.V.St   B.T.St   Barcode              DevType             
1     52557  -2698  31000  29000  30000  3283   3287   Dischg   Normal   Normal   Normal   57%      2024-09-06 09:42:07  Normal   Normal   ES1000899P021133     E-BOX-48100R-C     
2     52568  2767   31000  29000  30000  3281   3288   Charge   Normal   Normal   Normal   99%      2024-09-06 09:42:05  Normal   Normal   ES1000899P021107     E-BOX-48100R-C      
3     52566  -2763  30000  29000  29000  3283   3288   Dischg   Normal   Normal   Normal   5%       2024-09-06 09:42:04  Normal   Normal   ES1000899P021108     E-BOX-48100R-C      
4     52532  0      29000  27000  28000  3281   3286   Idle     Normal   Normal   Normal   100%      2024-09-06 09:42:04  Normal   Normal   ES1000899P021132     E-BOX-48100R-C      
5     -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
6     -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
7     -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
8     -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
9     -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
10    -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
11    -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
12    -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
13    -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
14    -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
15    -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
16    -      -      -      -      -      -      -      Absent   -        -        -        -        -                    -        -        -                    -                   
)";

using namespace pytes;

SCENARIO( "Raw BMS Telemetry can be parsed", "[bms::telemetry]" ) 
{
    GIVEN( "raw telemetry from pytes BMS serial console" ) 
    {
        const std::string telemetry{bmsOutput};

        WHEN( "parsing the telemetry" ) 
        {
            auto const parsedPowerTelemetry{bms::parseRawPowerTelemetry(telemetry)};

            THEN( "The number of parsed entries is as expected" ) 
            {
                REQUIRE( parsedPowerTelemetry.size() == 16 );
            }
            AND_THEN( "The parsed content matches the source" )
            {
                REQUIRE( parsedPowerTelemetry[0].curr_mA == -2698 );
                REQUIRE( parsedPowerTelemetry[0].base_state == bms::BatteryState::Discharging );
                REQUIRE( parsedPowerTelemetry[0].coulomb_percent == 57 );

                REQUIRE( parsedPowerTelemetry[1].curr_mA == 2767 );
                REQUIRE( parsedPowerTelemetry[1].base_state == bms::BatteryState::Charging );
                REQUIRE( parsedPowerTelemetry[1].coulomb_percent == 99 );

                REQUIRE( parsedPowerTelemetry[3].curr_mA == 0 );
                REQUIRE( parsedPowerTelemetry[3].base_state == bms::BatteryState::Idle );
                REQUIRE( parsedPowerTelemetry[3].coulomb_percent == 100 );

                REQUIRE( not parsedPowerTelemetry[4].curr_mA.has_value() );
                REQUIRE( parsedPowerTelemetry[4].base_state == bms::BatteryState::Absent );
                REQUIRE( not parsedPowerTelemetry[4].coulomb_percent.has_value() );
                REQUIRE( not parsedPowerTelemetry[4].volt_st.has_value() );
            }
        }
    }
}
