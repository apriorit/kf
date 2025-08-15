#include "pch.h"
#include <kf/EResource.h>
#include <kf/Thread.h>

namespace
{
    constexpr int kOneMillisecond = 10'000; // 1 ms in 100-nanosecond intervals

    void delay()
    {
        LARGE_INTEGER interval;
        interval.QuadPart = -kOneMillisecond;
        KeDelayExecutionThread(KernelMode, FALSE, &interval);
    }
}

SCENARIO("kf::EResource")
{
    GIVEN("An EResource object")
    {
        kf::EResource resource;

        THEN("The resource is initialized and not acquired immediately")
        {
            REQUIRE(resource.isAcquiredExclusive() == false);
            REQUIRE(resource.isAcquiredShared() == 0);
        }

        WHEN("The resource is acquired exclusively")
        {
            bool acquired = resource.acquireExclusive();

            //The system considers exclusive access to be a subset of shared access.
            //Therefore, a thread that has exclusive access to a resource also has shared access to the resource.
            THEN("The exclusive and shared locks are acquired")
            {
                REQUIRE(acquired == true);
                REQUIRE(resource.isAcquiredExclusive() == true);
                REQUIRE(resource.isAcquiredShared() == 1);
            }

            WHEN("The exclusive lock is released")
            {
                resource.release();

                THEN("The resource is no longer locked")
                {
                    REQUIRE(resource.isAcquiredExclusive() == false);
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
            REQUIRE(acquired == true);
            REQUIRE(resource.isAcquiredExclusive() == false);
            REQUIRE(resource.isAcquiredShared() == 1);
        }

        WHEN("The shared lock is released")
        {
            resource.release();

            THEN("The resource is no longer locked")
            {
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("The EResource acquired exclusive")
    {
        kf::EResource resource;
        bool exclusiveAcquired = resource.acquireExclusive();
        REQUIRE(exclusiveAcquired == true);

        WHEN("The resource is converted to shared")
        {
            resource.convertExclusiveToShared();

            THEN("The lock is converted to shared")
            {
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 1);
            }

            WHEN("The shared lock is released")
            {
                resource.release();

                THEN("The resource is no longer locked")
                {
                    REQUIRE(resource.isAcquiredExclusive() == false);
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
            REQUIRE(resource.isAcquiredExclusive() == true);
            REQUIRE(resource.isAcquiredShared() == 1);
        }

        WHEN("The resource is unlocked")
        {
            resource.unlock();

            THEN("The resource is no longer locked")
            {
                REQUIRE(resource.isAcquiredExclusive() == false);
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
            REQUIRE(resource.isAcquiredExclusive() == false);
            REQUIRE(resource.isAcquiredShared() == 1);
        }

        WHEN("The resource is unlocked shared")
        {
            resource.unlock_shared();

            THEN("The resource is no longer locked")
            {
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }

    GIVEN("The EResource and 2 threads")
    {

        struct Context
        {
            kf::EResource* resource;
            bool wasAcquiredByFirst = false;
            bool wasAcquiredBySecond = false;
        };

        WHEN("Multiple threads attempt to acquire exclusive the resource")
        {
            kf::EResource resource;
            kf::Thread thread1;
            kf::Thread thread2;

            Context context{ &resource };

            thread1.start([](void* context) {
                auto res = static_cast<Context*>(context);
                res->wasAcquiredByFirst = res->resource->acquireExclusive();
                delay();
                res->resource->release();
                },
                &context);

            thread2.start([](void* context) {
                auto res = static_cast<Context*>(context);
                res->wasAcquiredBySecond = res->resource->acquireExclusive();
                delay();
                res->resource->release();
                },
                &context);

            thread1.join();
            thread2.join();

            THEN("Both threads can acquire and release the resource without deadlock")
            {
                REQUIRE(context.wasAcquiredByFirst);
                REQUIRE(context.wasAcquiredBySecond);
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }

        WHEN("Multiple threads attempt to acquire shared the resource")
        {
            kf::EResource resource;
            kf::Thread thread1;
            kf::Thread thread2;

            Context context{ &resource };

            thread1.start([](void* context) {
                auto res = static_cast<Context*>(context);
                res->wasAcquiredByFirst = res->resource->acquireShared();
                delay();
                res->resource->release();
                },
                &context);

            thread2.start([](void* context) {
                auto res = static_cast<Context*>(context);

                res->wasAcquiredBySecond = res->resource->acquireShared();
                delay();
                res->resource->release();
                },
                &context);

            THEN("Both threads can acquire and release the resource without deadlock")
            {
                thread1.join();
                thread2.join();
                REQUIRE(context.wasAcquiredByFirst);
                REQUIRE(context.wasAcquiredBySecond);
            }
        }
    }
}
