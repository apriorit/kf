#include "pch.h"
#include <kf/EResource.h>
#include <kf/ThreadPool.h>
#include <kf/stl/vector>
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

    GIVEN("The EResource and multiple threads")
    {
        constexpr int kMaxThreadsCount = 64;
        const ULONG numLogicalProcessors = KeQueryActiveProcessorCount(nullptr);
        struct Context
        {
            kf::EResource* resource = nullptr;
            std::array<bool, kMaxThreadsCount>* acquired = nullptr;
            LONG counter = 0;
        };
        kf::EResource resource;
        std::array<bool, kMaxThreadsCount> acquired;
        Context context{ &resource, &acquired };
        kf::ThreadPool threadPool(numLogicalProcessors);

        WHEN("Multiple threads attempt to acquire exclusive the resource")
        {
            threadPool.start([](void* context) {
                auto res = static_cast<Context*>(context);
                res->resource->acquireExclusive();
                res->acquired->at(res->counter++) = true;
                delay();
                res->resource->release();
                }, &context);

            threadPool.join();

            THEN("All threads can acquire and release the resource without deadlock")
            {
                auto size = min(kMaxThreadsCount, numLogicalProcessors);
                for (size_t i = 0; i < size; i++)
                {
                    REQUIRE(acquired.at(i) == true);
                }
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }

        WHEN("Multiple threads attempt to acquire shared the resource")
        {
            threadPool.start([](void* context) {
                auto res = static_cast<Context*>(context);
                res->resource->acquireShared();
                auto index = InterlockedIncrement(&res->counter) - 1;
                res->acquired->at(index) = true;
                delay();
                res->resource->release();
                }, &context);

            threadPool.join();

            THEN("All threads can acquire and release the resource without deadlock")
            {
                auto size = min(kMaxThreadsCount, numLogicalProcessors);
                for (size_t i = 0; i < size; i++)
                {
                    REQUIRE(acquired.at(i) == true);
                }
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }
}
