#include "pch.h"
#include <kf/TimerThread.h>

namespace
{
    LARGE_INTEGER milliseconds(LONGLONG value)
    {
        LARGE_INTEGER interval{};
        interval.QuadPart = value * 10'000;
        return interval;
    }

    NTSTATUS waitForEvent(KEVENT& event)
    {
        auto timeout = milliseconds(-5'000);
        return KeWaitForSingleObject(&event, Executive, KernelMode, false, &timeout);
    }

    struct TimerContext
    {
        TimerContext()
        {
            KeInitializeEvent(&repeatedlyCalled, NotificationEvent, false);
        }

        void run()
        {
            if (++calls == 3)
            {
                KeSetEvent(&repeatedlyCalled, IO_NO_INCREMENT, false);
            }
        }

        std::atomic<int> calls = 0;
        KEVENT repeatedlyCalled = {};
    };
}

static_assert(std::is_default_constructible<kf::TimerThread>::value, "Timer threads must support default construction");
static_assert(!std::is_copy_constructible<kf::TimerThread>::value, "Timer threads must not be copy constructed");
static_assert(!std::is_copy_assignable<kf::TimerThread>::value, "Timer threads must not be copy assigned");
static_assert(std::is_nothrow_destructible<kf::TimerThread>::value, "Timer thread destruction must not throw");

SCENARIO("kf::TimerThread invokes periodic callbacks")
{
    GIVEN("A timer and an object with a callback")
    {
        TimerContext context;
        kf::TimerThread timer;

        WHEN("The timer is started with a positive period")
        {
            const NTSTATUS status = timer.start<&TimerContext::run>(&context, milliseconds(1));
            timer.join();

            THEN("Starting the timer succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
            }
        }

        WHEN("The timer runs until the third callback is signaled")
        {
            timer.start<&TimerContext::run>(&context, milliseconds(1));
            const NTSTATUS status = waitForEvent(context.repeatedlyCalled);
            timer.join();

            THEN("The callback is invoked repeatedly")
            {
                REQUIRE(status == STATUS_SUCCESS);
            }
        }
    }
}

SCENARIO("kf::TimerThread joins safely")
{
    GIVEN("A timer that has not been started")
    {
        kf::TimerThread timer;

        WHEN("The timer is joined")
        {
            timer.join();

            THEN("Joining returns without a worker")
            {
                REQUIRE(true);
            }
        }
    }

    GIVEN("A timer started with a one-minute period")
    {
        TimerContext context;
        kf::TimerThread timer;
        timer.start<&TimerContext::run>(&context, milliseconds(60'000));

        WHEN("The timer is joined before its first period expires")
        {
            timer.join();

            THEN("The pending callback is not invoked")
            {
                REQUIRE(context.calls == 0);
            }
        }

        WHEN("The timer is joined twice")
        {
            timer.join();
            timer.join();

            THEN("Joining again returns safely")
            {
                REQUIRE(true);
            }
        }
    }
}

SCENARIO("kf::TimerThread destruction joins the worker")
{
    GIVEN("A callback object that outlives the timer")
    {
        TimerContext context;

        WHEN("A timer with a pending one-minute period is destroyed")
        {
            {
                kf::TimerThread timer;
                timer.start<&TimerContext::run>(&context, milliseconds(60'000));
            }

            THEN("The pending callback is not invoked")
            {
                REQUIRE(context.calls == 0);
            }
        }
    }
}
