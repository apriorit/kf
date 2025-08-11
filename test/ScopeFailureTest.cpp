#include "pch.h"
#include <kf/ScopeFailure.h>

namespace
{
    static int g_functionCallCount = 0;

    void testFunction()
    {
        ++g_functionCallCount;
    }

}

SCENARIO("kf::MakeScopeFailure")
{
    GIVEN("A MakeScopeFailure with a valid function and unsuccessful NTSTATUS")
    {
        g_functionCallCount = 0;

        WHEN("The ScopeFailure object is created")
        {
            {
                NTSTATUS status = STATUS_ACCESS_DENIED;
                auto scopeExit = kf::MakeScopeFailure(status) += [] { testFunction(); };
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

    GIVEN("A MakeScopeFailure with a valid function and successful NTSTATUS")
    {
        g_functionCallCount = 0;

        WHEN("The ScopeFailure object is created")
        {
            {
                NTSTATUS status = STATUS_SUCCESS;
                auto scopeExit = kf::MakeScopeFailure(status) += [] { testFunction(); };
                THEN("The function is not called immediately")
                {
                    REQUIRE(g_functionCallCount == 0);
                }
            }

            THEN("The function is not called upon destruction")
            {
                REQUIRE(g_functionCallCount == 0);
            }
        }
    }

    GIVEN("SCOPE_FAILURE macro with an unsuccessful NTSTATUS")
    {
        NTSTATUS status = STATUS_ACCESS_DENIED;
        g_functionCallCount = 0;

        {
            SCOPE_FAILURE(status)
            {
                testFunction();
            };
        }

        THEN("Scoped function should be called")
        {
            REQUIRE(g_functionCallCount == 1);
        }
    }

    GIVEN("SCOPE_FAILURE macro with a successful NTSTATUS")
    {
        NTSTATUS status = STATUS_SUCCESS;
        g_functionCallCount = 0;

        {
            SCOPE_FAILURE(status)
            {
                testFunction();
            };
        }

        THEN("Scoped function should not be called")
        {
            REQUIRE(g_functionCallCount == 0);
        }
    }

    GIVEN("2 SCOPE_FAILURE macro with an unsuccessful NTSTATUS and 1 with successful")
    {
        NTSTATUS successful = STATUS_SUCCESS;
        NTSTATUS failed = STATUS_ACCESS_DENIED;
        g_functionCallCount = 0;

        {
            SCOPE_FAILURE(failed)
            {
                testFunction();
            };

            SCOPE_FAILURE(successful)
            {
                testFunction();
            };

            SCOPE_FAILURE(failed)
            {
                testFunction();
            };
        }

        THEN("Scoped function should be called twice")
        {
            REQUIRE(g_functionCallCount == 2);
        }
    }
}
