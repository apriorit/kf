#include "pch.h"
#include <kf/EResource.h>

SCENARIO("kf::EResource")
{
    GIVEN("An EResource object")
    {
        kf::EResource resource;

        THEN("The resource is initialized and not acquired immediately")
        {
            REQUIRE(!resource.isAcquiredExclusive());
            REQUIRE(resource.isAcquiredShared() == 0);
        }
    }

    GIVEN("The EResource acquired exclusive")
    {
        kf::EResource resource;
        bool acquired = resource.acquireExclusive();

        WHEN("The resource is acquired exclusively")
        {
            //The system considers exclusive access to be a subset of shared access.
            //Therefore, a thread that has exclusive access to a resource also has shared access to the resource.
            THEN("The exclusive and shared locks are acquired")
            {
                REQUIRE(acquired);
                REQUIRE(resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 1);
            }

            WHEN("The exclusive lock is released")
            {
                resource.release();

                THEN("The resource is no longer locked")
                {
                    REQUIRE(!resource.isAcquiredExclusive());
                    REQUIRE(resource.isAcquiredShared() == 0);
                }
            }
        }
    }

    GIVEN("The EResource is acquired shared")
    {
        kf::EResource resource;
        bool acquired = resource.acquireShared();

        THEN("The only shared lock is acquired")
        {
            REQUIRE(acquired);
            REQUIRE(!resource.isAcquiredExclusive());
            REQUIRE(resource.isAcquiredShared() == 1);
        }

        WHEN("The shared lock is released")
        {
            resource.release();

            THEN("The resource is no longer locked")
            {
                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("The EResource acquired exclusive")
    {
        kf::EResource resource;
        bool exclusiveAcquired = resource.acquireExclusive();
        REQUIRE(exclusiveAcquired);

        WHEN("The resource is converted to shared")
        {
            resource.convertExclusiveToShared();

            THEN("The lock is converted to shared")
            {
                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 1);
            }

            WHEN("The shared lock is released")
            {
                resource.release();

                THEN("The resource is no longer locked")
                {
                    REQUIRE(!resource.isAcquiredExclusive());
                    REQUIRE(resource.isAcquiredShared() == 0);
                }
            }
        }
    }

    GIVEN("The EResource locked using the lock function")
    {
        kf::EResource resource;
        resource.lock();

        THEN("The resource is acquired exclusively and shared")
        {
            REQUIRE(resource.isAcquiredExclusive());
            REQUIRE(resource.isAcquiredShared() == 1);
        }

        WHEN("The resource is unlocked")
        {
            resource.unlock();

            THEN("The resource is no longer locked")
            {
                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("The EResource is locked shared using the lock_shared function")
    {
        kf::EResource resource;
        resource.lock_shared();

        THEN("The resource is acquired shared")
        {
            REQUIRE(!resource.isAcquiredExclusive());
            REQUIRE(resource.isAcquiredShared() == 1);
        }

        WHEN("The resource is unlocked shared")
        {
            resource.unlock_shared();

            THEN("The resource is no longer locked")
            {
                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("The EResource acquired shared twice by the current thread")
    {
        kf::EResource resource;
        REQUIRE(resource.acquireShared());
        REQUIRE(resource.acquireShared());

        WHEN("The shared acquires are released one at a time")
        {
            THEN("One release leaves acquire count 1 and the second clears it")
            {
                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 2);

                resource.release();

                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 1);

                resource.release();

                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("The EResource acquired exclusive twice by the current thread")
    {
        kf::EResource resource;
        REQUIRE(resource.acquireExclusive());
        REQUIRE(resource.acquireExclusive());

        WHEN("One exclusive acquire is released")
        {
            THEN("The resource stays exclusively owned")
            {
                REQUIRE(resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 2);

                resource.release();

                REQUIRE(resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 1);

                resource.release();

                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("The EResource acquired exclusive twice")
    {
        kf::EResource resource;
        REQUIRE(resource.acquireExclusive());
        REQUIRE(resource.acquireExclusive());

        WHEN("The exclusive acquire is converted to shared")
        {
            resource.convertExclusiveToShared();

            THEN("Exclusive ownership is cleared and the recursive owner count is kept")
            {
                // Conversion clears the exclusive flag but does not drop recursive acquires.
                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 2);
                REQUIRE(resource.getOwnerCount() == 2);

                resource.release();
                resource.release();

                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("A free EResource")
    {
        kf::EResource resource;

        WHEN("It is acquired exclusive without waiting")
        {
            const bool acquired = resource.acquireExclusive(false);

            THEN("The non-blocking exclusive acquire succeeds")
            {
                REQUIRE(acquired);
                REQUIRE(resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 1);
                REQUIRE(resource.getOwnerCount() == 1);

                resource.release();
            }
        }

        WHEN("It is acquired shared without waiting")
        {
            const bool acquired = resource.acquireShared(false);

            THEN("The non-blocking shared acquire succeeds")
            {
                REQUIRE(acquired);
                REQUIRE(!resource.isAcquiredExclusive());
                REQUIRE(resource.isAcquiredShared() == 1);
                REQUIRE(resource.getOwnerCount() == 1);

                resource.release();
            }
        }
    }
}
