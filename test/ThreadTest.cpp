#include "pch.h"
#include <kf/Thread.h>

namespace
{
    struct TestObject
    {
        NTSTATUS run()
        {
            value = 123;
            return STATUS_SUCCESS;
        }

        int value = 0;
    };
}

SCENARIO("kf::Thread")
{
    GIVEN("Default-constructed thread")
    {
        kf::Thread thread;

        WHEN("Starting a thread with lambda")
        {
            int value = 0;
            NTSTATUS status = thread.start([](void* context) {
                auto p = static_cast<int*>(context);
                *p = 123;
                }, &value);

            THEN("Status is successful")
            {
                REQUIRE(NT_SUCCESS(status));
            }

            thread.join();

            THEN("Thread changed the given value")
            {
                REQUIRE(value == 123);
            }
        }

        WHEN("Thread is not started and joined")
        {
            thread.join();

            THEN("Does nothing and no crash occurs")
            {
            }
        }

        WHEN("Calling join twice after start")
        {
            REQUIRE_NT_SUCCESS(thread.start([](void*) { }, nullptr));

            thread.join();
            thread.join();

            THEN("Does nothing and no crash occurs")
            {
            }
        }
    }

    GIVEN("Thread with a member routine")
    {
        kf::Thread thread;
        TestObject obj;

        WHEN("Starting a thread with member function")
        {
            NTSTATUS status = thread.start<&TestObject::run>(&obj);

            THEN("Status is successful")
            {
                REQUIRE(NT_SUCCESS(status));
            }

            thread.join();

            THEN("Thread called the function")
            {
                REQUIRE(obj.value == 123);
            }
        }
    }

    GIVEN("Thread with long working function")
    {
        kf::Thread thread;
        int value = 0;

        WHEN("Starting a thread with long working function and joined")
        {
            REQUIRE_NT_SUCCESS(thread.start([](void* context) {
                auto p = static_cast<int*>(context);
                LARGE_INTEGER interval;
                interval.QuadPart = -10'000;
                KeDelayExecutionThread(KernelMode, FALSE, &interval);
                *p = 246;
                }, &value));

            thread.join();

            THEN("Main thread execution continued after given thread finished its work")
            {
                REQUIRE(value == 246);
            }
        }
    }

    GIVEN("Thread that goes out of scope")
    {
        int value = 0;
        {
            kf::Thread thread;
            REQUIRE_NT_SUCCESS(thread.start([](void* context) {
                auto p = static_cast<int*>(context);
                *p = 777;
                }, &value));
        }

        THEN("Destructor joined and work was finished")
        {
            REQUIRE(value == 777);
        }
    }

    GIVEN("Working thread")
    {
        kf::Thread thread;
        int value = 0;

        REQUIRE_NT_SUCCESS(thread.start([](void* context) {
            auto p = static_cast<int*>(context);
            LARGE_INTEGER interval;
            interval.QuadPart = -10'000;
            KeDelayExecutionThread(KernelMode, FALSE, &interval);
            *p = 999;
            }, &value));

        WHEN("Thread is moved to another instance")
        {
            kf::Thread thread2 = std::move(thread);
            thread2.join();

            THEN("Function form thread is completed")
            {
                REQUIRE(value == 999);
            }
        }
    }
}
