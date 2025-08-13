#include "pch.h"
#include <kf/ScopeExit.h>

SCENARIO("SCOPE_EXIT macro")
{
    GIVEN("A block of code with SCOPE_EXIT")
    {
        WHEN("The block is executed")
        {
            int value = 0;

            {
                SCOPE_EXIT{ value++; };

                THEN("Scoped function doesn't execute until scope ends")
                {
                    REQUIRE(value == 0);
                }
            }

            THEN("The cleanup code should execute after the block")
            {
                REQUIRE(value == 1);
            }
        }

        WHEN("Multiple SCOPE_EXIT macros are used in the same block")
        {
            int value = 1;

            {
                SCOPE_EXIT
                {
                    value *= 5;
                };

                SCOPE_EXIT
                {
                    value += 2;
                };
            }

            THEN("The cleanup code should execute in reverse order of declaration")
            {
                REQUIRE(value == 15);
            }
        }
    }
}
