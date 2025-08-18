#include "pch.h"
#include <kf/Event.h>
#include <kf/Thread.h>

SCENARIO("kf::Event")
{
    constexpr auto kOneMillisecond = 10'000; // 1ms

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

    GIVEN("NotificationEvent with multiple waiters and NotificationEvent for complition")
    {
        kf::Event triggerEvent(NotificationEvent, false);
        kf::Event completionEvent1(NotificationEvent, false);
        kf::Event completionEvent2(NotificationEvent, false);
        std::pair context1{ &triggerEvent, &completionEvent1 };
        std::pair context2{ &triggerEvent, &completionEvent2 };
        kf::Thread thread1;
        kf::Thread thread2;

        thread1.start([](void* context) {
            const auto e = static_cast<std::pair<kf::Event*, kf::Event*>*>(context);
            e->first->wait();
            e->second->set();
            }, &context1);

        thread2.start([](void* context) {
            const auto e = static_cast<std::pair<kf::Event*, kf::Event*>*>(context);
            e->first->wait();
            e->second->set();
            }, &context2);

        WHEN("A wait is performed with no timeout and the event is not set")
        {
            THEN("Both threads are waiting")
            {
                REQUIRE(!completionEvent1.isSet());
                REQUIRE(!completionEvent2.isSet());
            }

            triggerEvent.set();
            completionEvent1.wait();
            completionEvent2.wait();

            THEN("After trigger event is set, both threads are released from waiting")
            {
                REQUIRE(completionEvent1.isSet());
                REQUIRE(completionEvent2.isSet());
            }
        }
    }

    GIVEN("SynchronizationEvent with multiple waiters and NotificationEvent for complition")
    {
        kf::Event triggerEvent(SynchronizationEvent, false);
        kf::Thread thread1;
        kf::Thread thread2;

        kf::Event completionEvent1(NotificationEvent, false);
        kf::Event completionEvent2(NotificationEvent, false);
        std::pair firstContext{ &triggerEvent, &completionEvent1 };
        std::pair secondContext{ &triggerEvent, &completionEvent2 };

        thread1.start([](void* context) {
            const auto data = static_cast<std::pair<kf::Event*, kf::Event*>*>(context);
            data->first->wait();
            data->second->set();
            }, &firstContext);

        thread2.start([](void* context) {
            const auto data = static_cast<std::pair<kf::Event*, kf::Event*>*>(context);
            data->first->wait();
            data->second->set();
            }, &secondContext);

        THEN("Both threads are waiting")
        {
            REQUIRE(!completionEvent1.isSet());
            REQUIRE(!completionEvent2.isSet());
        }

        triggerEvent.set();

        THEN("Only one thread is released and event resets")
        {
            REQUIRE(completionEvent1.isSet() ^ completionEvent2.isSet()); // Exactly one is true
            REQUIRE(!triggerEvent.isSet());
        }

        triggerEvent.set();
    }
}
