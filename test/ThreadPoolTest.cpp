#include "pch.h"
#include <kf/ThreadPool.h>

SCENARIO("kf::ThreadPool")
{
    struct TestObject
    {
        NTSTATUS run()
        {
            InterlockedIncrement(&value);
            return STATUS_SUCCESS;
        }

        LONG value = 0;
    };

    constexpr auto fn = [](void* context) {
        LARGE_INTEGER interval;
        interval.QuadPart = -10'000; 
        KeDelayExecutionThread(KernelMode, FALSE, &interval);
        auto p = static_cast<LONG*>(context);
        InterlockedIncrement(p);
        };

    GIVEN("A ThreadPool with count <= kMaxCount")
    {
        kf::ThreadPool pool(4);

        WHEN("Starting threads with a lambda")
        {
            LONG value = 0;
            NTSTATUS status = pool.start(fn, &value);

            THEN("Status is successful")
            {
                REQUIRE(NT_SUCCESS(status));
            }

            pool.join();

            THEN("All threads incremented the value")
            {
                REQUIRE(value == 4);
            }
        }

        WHEN("Starting threads with a member function")
        {
            TestObject obj;
            NTSTATUS status = pool.start<&TestObject::run>(&obj);

            THEN("Status is successful")
            {
                REQUIRE(NT_SUCCESS(status));
            }

            pool.join();

            THEN("All threads executed the member routine")
            {
                REQUIRE(obj.value == 4);
            }
        }
    }

    GIVEN("A ThreadPool with count > kMaxCount")
    {
        kf::ThreadPool pool(100);
        LONG value = 0;

        WHEN("Starting threads")
        {
            REQUIRE_NT_SUCCESS(pool.start(fn, &value));

            pool.join();

            THEN("ThreadPool started up to kMaxCount threads")
            {
                REQUIRE(value == 64);
            }
        }
    }

    GIVEN("A ThreadPool with 4 threads")
    {
        LONG value = 0;

        WHEN("ThreadPool with started threads goes out of scope")
        {
            {
                kf::ThreadPool pool(4);
                REQUIRE_NT_SUCCESS(pool.start(fn, &value));
            }

            THEN("All threads are complete successfully")
            {
                REQUIRE(value == 4);
            }
        }
    }

    GIVEN("ThreadPool with 4 started threads")
    {
        LONG value = 0;
        kf::ThreadPool pool1(4);

        REQUIRE_NT_SUCCESS(pool1.start(fn, &value));

        WHEN("ThreadPool moved into another pool")
        {
            kf::ThreadPool pool2(std::move(pool1));
            pool2.join();

            THEN("Threads still run and complete successfully")
            {
                REQUIRE(value == 4);
            }
        }
    }
}
