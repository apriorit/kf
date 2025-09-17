#include "pch.h"
#include <kf/Event.h>
#include <kf/Thread.h>

SCENARIO("kf::Event")
{
    constexpr auto kOneMillisecond = 10'000; // 1ms
    LARGE_INTEGER kZeroTimeout{0};

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

    GIVEN("2 NotificationEvents for triggering and compition with false state")
    {
        kf::Event triggerEvent(NotificationEvent, false);
        kf::Event completionEvent(NotificationEvent, false);
        std::pair<kf::Event*, kf::Event*> events{ &triggerEvent, &completionEvent };
        kf::Thread thread;

        WHEN("A wait is performed with no timeout and the event is not set")
        {
            thread.start([](void* context) {
                const auto events = static_cast<std::pair<kf::Event*, kf::Event*>*>(context);
                events->first->wait();
                events->second->set();
                }, &events);

            THEN("Event is not set immediately")
            {
                REQUIRE(!completionEvent.isSet());
            }

            triggerEvent.set();

            THEN("After trigger event is set, completionEvent from thread is set too")
            {
                REQUIRE(completionEvent.wait() == STATUS_SUCCESS);
                REQUIRE(completionEvent.isSet());
            }
        }

        WHEN("A wait is performed with a timeout and the event is not set")
        {
            kf::Event ev(NotificationEvent, false);
            LARGE_INTEGER timeout;
            timeout.QuadPart = -kOneMillisecond;

            const auto status = ev.wait(&timeout);

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

            THEN("Only one wait is successed and event resets")
            {
                auto status1 = event.wait(&kZeroTimeout);
                auto status2 = event.wait(&kZeroTimeout);
                REQUIRE_NT_SUCCESS(status1);
                REQUIRE(status2 == STATUS_TIMEOUT);
                REQUIRE(!event.isSet());
            }
        }
    }
}
