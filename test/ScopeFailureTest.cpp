#include "pch.h"
#include <kf/ScopeFailure.h>

SCENARIO("SCOPE_FAILURE macro")
{
    GIVEN("SCOPE_FAILURE macro with an unsuccessful NTSTATUS")
    {
        NTSTATUS status = STATUS_ACCESS_DENIED;
        int value = 0;

        {
            SCOPE_FAILURE(status)
            {
                value++;
            };
        }

        THEN("Scoped function should be called")
        {
            REQUIRE(value == 1);
        }
    }

    GIVEN("SCOPE_FAILURE macro with a successful NTSTATUS")
    {
        NTSTATUS status = STATUS_SUCCESS;
        int value = 0;

        {
            SCOPE_FAILURE(status)
            {
                value++;
            };
        }

        THEN("Scoped function should not be called")
        {
            REQUIRE(value == 0);
        }
    }

    GIVEN("2 SCOPE_FAILURE macro with an unsuccessful NTSTATUS and 1 with successful")
    {
        NTSTATUS successful = STATUS_SUCCESS;
        NTSTATUS failed = STATUS_ACCESS_DENIED;
        int value = 0;

        {
            SCOPE_FAILURE(failed)
            {
                value++;
            };

            SCOPE_FAILURE(successful)
            {
                value++;
            };

            SCOPE_FAILURE(failed)
            {
                value++;
            };
        }

        THEN("Scoped function should be called twice")
        {
            REQUIRE(value == 2);
        }
    }
}
