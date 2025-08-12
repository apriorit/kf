#include "pch.h"
#include <kf/ScopeExit.h>

SCENARIO("SCOPE_EXIT macro")
{
    GIVEN("A block of code with SCOPE_EXIT")
    {
        WHEN("The block is executed")
        {
            int a = 0;

            {
                SCOPE_EXIT{ a++; };
            }

            THEN("The cleanup code should execute after the block")
            {
                REQUIRE(a == 1);
            }
        }

        WHEN("Multiple SCOPE_EXIT macros are used in the same block")
        {
            int a = 1;

            {
                SCOPE_EXIT
                {
                    a *= 5;
                };

                SCOPE_EXIT
                {
                    a += 2;
                };
            }

            THEN("The cleanup code should execute in reverse order of declaration")
            {
                REQUIRE(a == 15);
            }
        }
    }
}
