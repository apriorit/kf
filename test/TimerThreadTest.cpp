#include "pch.h"
#include <kf/TimerThread.h>

namespace
{
    void delay(LONGLONG time)
    {
        LARGE_INTEGER interval;
        interval.QuadPart = -time;
        KeDelayExecutionThread(KernelMode, FALSE, &interval);
    }
}

SCENARIO("TimerThread callback execution")
{
    constexpr auto k100Microsecond = 1'000; // 100 microseconds
    constexpr auto kMillisecond = 10'000;

    GIVEN("TimerThread and a callback incrementing a counter")
    {
        kf::TimerThread timer;
        LONG counter = 0;

        LARGE_INTEGER period;
        period.QuadPart = k100Microsecond;

        REQUIRE_NT_SUCCESS(timer.start(period, [&counter]() {
            InterlockedIncrement(&counter);
            }));

        WHEN("Waiting enough time for multiple callbacks")
        {
            delay(5 * kMillisecond); // 5ms

            THEN("Counter should have been incremented at least greater than 1")
            {
                REQUIRE(counter > 1);
            }
        }

        WHEN("Stopping the TimerThread via join")
        {
            delay(5 * kMillisecond); // 5 ms
            timer.join();
            int valueAfterJoin = counter;
            delay(10 * kMillisecond); // 10 ms

            THEN("Counter should not increase after join")
            {
                REQUIRE(counter == valueAfterJoin);
            }
        }
    }

    GIVEN("TimerThread in a scope and a callback incrementing a counter")
    {
        LONG counter = 0;

        {
            kf::TimerThread timer;
            LARGE_INTEGER period;
            period.QuadPart = k100Microsecond;

            REQUIRE_NT_SUCCESS(timer.start(period, [&counter]() {
                InterlockedIncrement(&counter);
                }));

            delay(5 * kMillisecond);
            REQUIRE(counter > 1);
        }

        int valueAfterScope = counter;
        
        WHEN("TimerThread goes out of scope")
        {
            delay(10 * kMillisecond); // 10 ms

            THEN("Counter should not increase any more")
            {
                REQUIRE(counter == valueAfterScope);
            }
        }
    }
}
