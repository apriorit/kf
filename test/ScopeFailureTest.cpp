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

            THEN("Scoped function doesn't call until scope not ended")
            {
                REQUIRE(value == 0);
            }
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

    GIVEN("SCOPE_FAILURE macro with different NTSTATUS")
    {
        NTSTATUS status = STATUS_SUCCESS;
        NTSTATUS scopedStatus;
        int value = 0;

        {
            status = STATUS_ACCESS_DENIED;

            SCOPE_FAILURE(status)
            {
                value++;
                scopedStatus = status;;
            };

            status = STATUS_CANT_WAIT;
        }

        THEN("Scoped function should be called with last NTSTATUS")
        {
            REQUIRE(value == 1);
            REQUIRE(scopedStatus == STATUS_CANT_WAIT);
        }
    }
}
