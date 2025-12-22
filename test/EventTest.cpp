#include "pch.h"
#include <kf/Event.h>
#include <kf/Thread.h>

SCENARIO("kf::Event")
{
    constexpr LARGE_INTEGER kOneMillisecond{ .QuadPart = -10'000LL }; // 1ms
    constexpr LARGE_INTEGER kZeroTimeout{};

    GIVEN("NotificationEvent with true state")
    {
        kf::Event ev(NotificationEvent, true);

        THEN("The event is initially set")
        {
            REQUIRE(ev.isSet());
        }

        WHEN("The event is cleared")
        {
            ev.clear();

            THEN("The event is no longer set")
            {
                REQUIRE(!ev.isSet());
            }
        }

        WHEN("The event is cleared multiple times")
        {
            ev.clear();
            ev.clear();
            ev.clear();

            THEN("The event remains unsignaled")
            {
                REQUIRE(!ev.isSet());
            }
        }

        WHEN("The event is set")
        {
            ev.set();

            THEN("The event is in a signaled state")
            {
                REQUIRE(ev.isSet());
            }
        }

        WHEN("The event is set multiple times")
        {
            ev.set();
            ev.set();

            THEN("The event remains in a signaled state")
            {
                REQUIRE(ev.isSet());
            }
        }
    }

    GIVEN("SynchronizationEvent with false state")
    {
        kf::Event ev(SynchronizationEvent, false);

        THEN("The event is initially not set")
        {
            REQUIRE(!ev.isSet());
        }

        WHEN("The event is set")
        {
            ev.set();

            THEN("The event is in a signaled state")
            {
                REQUIRE(ev.isSet());
            }
        }
    }

    GIVEN("NotificationEvent with true state")
    {
        kf::Event event(NotificationEvent, true);

        WHEN("A wait is performed with no timeout when event is set")
        {
            THEN("The wait returns with STATUS_SUCCESS and remains in set state")
            {
                REQUIRE(event.wait() == STATUS_SUCCESS);
                REQUIRE(event.isSet());
            }
        }

        WHEN("A wait is performed with a timeout and the event is not set")
        {
            kf::Event ev(NotificationEvent, false);

            const auto status = ev.wait(&kOneMillisecond);

            THEN("The wait times out")
            {
                REQUIRE(status == STATUS_TIMEOUT);
                REQUIRE(!ev.isSet());
            }
        }
    }

    GIVEN("NotificationEvent with multiple wait() calls")
    {
        kf::Event event(NotificationEvent, false);

        WHEN("A wait is performed with 0 and the event is not set")
        {
            THEN("Both waits are timed out")
            {
                REQUIRE(STATUS_TIMEOUT == event.wait(&kZeroTimeout));
                REQUIRE(STATUS_TIMEOUT == event.wait(&kZeroTimeout));
            }

            event.set();

            THEN("After event is set, both waits are successed")
            {
                REQUIRE(STATUS_SUCCESS == event.wait(&kZeroTimeout));
                REQUIRE(STATUS_SUCCESS == event.wait(&kZeroTimeout));
            }
        }
    }

    GIVEN("SynchronizationEvent with multiple wait() calls")
    {
        kf::Event event(SynchronizationEvent, false);

        WHEN("A wait is performed with 0 and the event is not set")
        {
            THEN("Both waits are timed out")
            {
                REQUIRE(STATUS_TIMEOUT == event.wait(&kZeroTimeout));
                REQUIRE(STATUS_TIMEOUT == event.wait(&kZeroTimeout));
            }

            event.set();

            THEN("The first wait succeeds, the second timeouts and event resets")
            {
                REQUIRE(STATUS_SUCCESS == event.wait(&kZeroTimeout));
                REQUIRE(!event.isSet());
                REQUIRE(STATUS_TIMEOUT == event.wait(&kZeroTimeout));
                REQUIRE(!event.isSet());
            }
        }
    }
}
