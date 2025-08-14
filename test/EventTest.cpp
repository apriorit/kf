#include "pch.h"
#include <kf/Event.h>
#include <kf/Thread.h>

SCENARIO("kf::Event")
{
    constexpr auto kOneSecond = 10'000'000; // 1 second in 100-nanosecond intervals

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

    GIVEN("NotificationEvent with false state")
    {
        WHEN("A wait is performed with no timeout and the event is not set")
        {
            kf::Event ev(NotificationEvent, false);
            kf::Thread thread;
            thread.start([](void* context) {
                auto e = static_cast<kf::Event*>(context);
                LARGE_INTEGER delay;
                delay.QuadPart = -kOneSecond;
                KeDelayExecutionThread(KernelMode, false, &delay);
                e->set();
                }, &ev);

            THEN("Event is not set immediately")
            {
                REQUIRE(!ev.isSet());
            }

            auto status = ev.wait();

            THEN("The wait succeeds when the event is set")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ev.isSet());
            }
        }

        WHEN("A wait is performed with a timeout and the event is not set")
        {
            kf::Event ev(NotificationEvent, false);
            LARGE_INTEGER timeout;
            timeout.QuadPart = -kOneSecond;

            auto status = ev.wait(&timeout);

            THEN("The wait times out")
            {
                REQUIRE(status == STATUS_TIMEOUT);
                REQUIRE(!ev.isSet());
            }
        }
    }

    GIVEN("NotificationEvent with multiple waiters")
    {
        WHEN("A wait is performed with no timeout and the event is not set")
        {
            kf::Event ev(NotificationEvent, false);
            kf::Thread thread1;
            kf::Thread thread2;
            bool firstWoken = false;
            bool secondWoken = false;

            std::pair firstContext{ &ev, &firstWoken };
            std::pair secondContext{ &ev, &secondWoken };

            thread1.start([](void* context) {
                auto e = static_cast<std::pair<kf::Event*, bool*>*>(context);
                e->first->wait();
                *e->second = true;
                }, &firstContext);

            thread2.start([](void* context) {
                auto e = static_cast<std::pair<kf::Event*, bool*>*>(context);
                e->first->wait();
                *e->second = true;
                }, &secondContext);

            THEN("Both threads are waiting")
            {
                REQUIRE(!firstWoken);
                REQUIRE(!secondWoken);
            }

            ev.set();

            // Give some time for threads to wake up
            LARGE_INTEGER delay;
            delay.QuadPart = -kOneSecond / 2;
            KeDelayExecutionThread(KernelMode, false, &delay);

            THEN("Both threads are released from waiting")
            {
                REQUIRE(firstWoken);
                REQUIRE(secondWoken);
            }
        }
    }
    GIVEN("SynchronizationEvent with multiple waiters")
    {
        kf::Event ev(SynchronizationEvent, false);
        kf::Thread thread1;
        kf::Thread thread2;

        bool firstWoken = false;
        bool secondWoken = false;
        std::pair firstContext{ &ev, &firstWoken };
        std::pair secondContext{ &ev, &secondWoken };

        thread1.start([](void* context) {
            auto data = static_cast<std::pair<kf::Event*, bool*>*>(context);
            data->first->wait();
            *data->second = true;
            }, &firstContext);

        thread2.start([](void* context) {
            auto data = static_cast<std::pair<kf::Event*, bool*>*>(context);
            data->first->wait();
            *data->second = true;
            }, &secondContext);

        THEN("Both threads are waiting")
        {
            REQUIRE(!firstWoken);
            REQUIRE(!secondWoken);
        }

        ev.set();
        // Give some time for threads to wake up
        LARGE_INTEGER delay;
        delay.QuadPart = -kOneSecond / 2;
        KeDelayExecutionThread(KernelMode, false, &delay);

        THEN("Only one thread is released and event resets")
        {
            REQUIRE(firstWoken ^ secondWoken); // Exactly one is true
            REQUIRE(!ev.isSet());
        }

        ev.set();
    }
}
