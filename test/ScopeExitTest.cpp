#include "pch.h"
#include <kf/ScopeExit.h>
#include <kf/USimpleString.h>

static int g_functionCallCount = 0;

void testFunction()
{
    ++g_functionCallCount;
}

SCENARIO("kf::ScopeExit")
{
    GIVEN("A ScopeExit with a valid function")
    {
        g_functionCallCount = 0;

        WHEN("The ScopeExit object is created")
        {
            {
                auto scopeExit = kf::MakeScopeExit() += [] { testFunction(); };
                THEN("The function is not called immediately")
                {
                    REQUIRE(g_functionCallCount == 0);
                }
            }

            THEN("The function is called upon destruction")
            {
                REQUIRE(g_functionCallCount == 1);
            }
        }
    }
}

SCENARIO("SCOPE_EXIT macro")
{
    GIVEN("A block of code with SCOPE_EXIT")
    {
        g_functionCallCount = 0;

        WHEN("The block is executed")
        {
            {
                SCOPE_EXIT { testFunction(); };
            }

            THEN("The cleanup code should execute after the block")
            {
                REQUIRE(g_functionCallCount == 1);
            }
        }

        WHEN("Multiple SCOPE_EXIT macros are used in the same block")
        {
            kf::USimpleString str;
            kf::USimpleString expectedStr = L"Second scope exit. First scope exit.";

            {
                SCOPE_EXIT
                {
                    str.concat(L"First scope exit.");
                };

                SCOPE_EXIT
                {
                    str.concat(L"Second scope exit. ");
                };
            }

            THEN("The cleanup code should execute in reverse order of declaration")
            {
                REQUIRE(str.equals(expectedStr));
            }
        }
    }
}
