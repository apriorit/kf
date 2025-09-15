#include "pch.h"
#include <kf/ConditionVariable.h>
#include <kf/Thread.h>
#include <kf/EResource.h>

namespace
{
    struct Context
    {
        kf::EResource* resource = nullptr;
        kf::ConditionVariable* cv = nullptr;
        LONG counter = 0;
    };

    void delay()
    {
        LARGE_INTEGER interval;
        interval.QuadPart = -10'000; // 1ms
        KeDelayExecutionThread(KernelMode, FALSE, &interval);
    }
}

SCENARIO("kf::ConditionVariable")
{
    GIVEN("ConditionVariable without timeout and 1 thread")
    {
        WHEN("waitFor: timeout is nullptr and cv is notified")
        {
            kf::EResource resource;
            kf::ConditionVariable cv;
            kf::Thread thread;
            thread.start([](void* context) {
                auto cv = static_cast<kf::ConditionVariable*>(context);
                delay();
                cv->notifyOne();
                }, &cv);

            auto status = cv.waitFor(resource, nullptr);
            THEN("waitFor should return success")
            {
                REQUIRE(status == kf::ConditionVariable::Status::Success);
            }
        }

        WHEN("waitFor: timeout is set to 0")
        {
            kf::EResource resource;
            kf::ConditionVariable cv;
            LARGE_INTEGER timeout{ 0 };
            auto status = cv.waitFor(resource, &timeout);
            THEN("waitFor should return Status::Timeout")
            {
                REQUIRE(status == kf::ConditionVariable::Status::Timeout);
            }
        }

        WHEN("waitFor: timeout is set to 1ms")
        {
            kf::EResource resource;
            kf::ConditionVariable cv;
            LARGE_INTEGER timeout;
            timeout.QuadPart = -10'000; // 1ms
            auto status = cv.waitFor(resource, &timeout);
            THEN("waitFor should return Status::Timeout")
            {
                REQUIRE(status == kf::ConditionVariable::Status::Timeout);
            }
        }
    }

    GIVEN("ConditionVariable wait with predicate immediately true")
    {
        kf::EResource resource;
        kf::ConditionVariable cv;
        LONG counter = 1;

        cv.wait(resource, [&]() { return counter > 0; });
        THEN("Wait should return immediately")
        {
            REQUIRE(counter == 1);
        }
    }

    GIVEN("ConditionVariable without timeout and 2 Threads waiting for condition variable")
    {
        kf::EResource resource;
        kf::ConditionVariable cv;
        Context ctx{ &resource, &cv };
        kf::Thread thread1;
        kf::Thread thread2;

        thread1.start([](void* context) {
            auto ctx = static_cast<Context*>(context);
            ctx->cv->wait(*ctx->resource, [&]() {
                return ctx->counter > 0; });
            ctx->counter++;
            ctx->resource->release();
            }, &ctx);

        thread2.start([](void* context) {
            auto ctx = static_cast<Context*>(context);
            ctx->cv->wait(*ctx->resource, [&]() {
                return ctx->counter > 0; });
            ctx->counter++;
            ctx->resource->release();
            }, &ctx);
        delay();

        WHEN("Predicate is true and called notifyAll")
        {
            ctx.counter++;
            cv.notifyAll();
            thread1.join();
            thread2.join();

            THEN("Both threads should wake up and increment counter")
            {
                REQUIRE(ctx.counter == 3);
            }
        }
    }

    GIVEN("ConditionVariable without timeout and 1 thread waiting for predicate")
    {
        kf::EResource resource;
        kf::ConditionVariable cv;
        Context ctx{ &resource, &cv };
        kf::Thread thread;

        WHEN("Thread is notified")
        {
            thread.start([](void* context) {
                auto ctx = static_cast<Context*>(context);
                ctx->cv->wait(*ctx->resource, [&]() { return ctx->counter > 0; });
                ctx->counter++;
                ctx->resource->release();
                }, &ctx);

            delay();

            THEN("Thread should be waiting for predicate")
            {
                REQUIRE(ctx.counter == 0);
            }

            ctx.counter++;
            cv.notifyOne();
            thread.join();

            THEN("Predicate become true and thread should wake up")
            {
                REQUIRE(ctx.counter == 2);
            }
        }
    }

    GIVEN("ConditionVariable with 2 waiting threads and notifyOne is called")
    {
        kf::EResource resource;
        kf::ConditionVariable cv;
        Context ctx{ &resource, &cv };
        kf::Thread thread1;
        kf::Thread thread2;

        thread1.start([](void* context) {
            auto ctx = static_cast<Context*>(context);
            ctx->cv->wait(*ctx->resource, [&]() { return ctx->counter > 0; });
            ctx->counter++;
            ctx->resource->release();
            }, &ctx);

        thread2.start([](void* context) {
            auto ctx = static_cast<Context*>(context);
            ctx->cv->wait(*ctx->resource, [&]() { return ctx->counter > 0; });
            ctx->counter++;
            ctx->resource->release();
            }, &ctx);

        delay();
        ctx.counter++;
        cv.notifyOne();
        delay();

        THEN("Only one thread should wake up and increment counter")
        {
            REQUIRE(ctx.counter == 2);
        }

        cv.notifyOne();
        thread1.join();
        thread2.join();

        THEN("After second notifyOne both threads should finish")
        {
            REQUIRE(ctx.counter == 3);
        }
    }

    GIVEN("ConditionVariable::waitFor with predicate and timeout")
    {
        WHEN("Timeout is nullptr and predicate is true")
        {
            kf::EResource resource;
            kf::ConditionVariable cv;
            kf::Thread thread;
            int counter = 1;
            thread.start([](void* context) {
                auto cv = static_cast<kf::ConditionVariable*>(context);
                delay();
                cv->notifyOne();
                }, &cv);

            THEN("waitFor should return true")
            {
                REQUIRE(cv.waitFor(resource, nullptr, [&]() { return counter > 0; }));
            }
        }

        WHEN("Timeout is set to 0 and predicate is false")
        {
            kf::EResource resource;
            kf::ConditionVariable cv;
            LARGE_INTEGER timeout{ 0 };
            int counter = 0;
            THEN("waitFor should return false")
            {
                REQUIRE(!cv.waitFor(resource, &timeout, [&]() { return counter > 0; }));
            }
        }

        WHEN("Timeout is set to 1ms and predicate is false")
        {
            kf::EResource resource;
            kf::ConditionVariable cv;
            LARGE_INTEGER timeout;
            timeout.QuadPart = -10'000; // 1ms
            int counter = 0;
            THEN("waitFor should return false")
            {
                REQUIRE(!cv.waitFor(resource, &timeout, [&]() { return counter > 0; }));
            }
        }
    }

    GIVEN("ConditionVariable with no waiters")
    {
        kf::EResource resource;
        kf::ConditionVariable cv;

        WHEN("notifyAll is called without waiters")
        {
            cv.notifyAll();
            THEN("Nothing should happen")
            {
            }
        }

        WHEN("notifyOne is called without waiters")
        {
            cv.notifyOne();
            THEN("Nothing should happen")
            {
            }
        }
    }

    GIVEN("ConditionVariable with 1 thread and multiple notifyOne calls")
    {
        kf::EResource resource;
        kf::ConditionVariable cv;
        Context ctx{ &resource, &cv };
        kf::Thread thread;

        thread.start([](void* context) {
            auto ctx = static_cast<Context*>(context);
            ctx->cv->wait(*ctx->resource, [&]() { return ctx->counter > 0; });
            ctx->counter++;
            ctx->resource->release();
            }, &ctx);

        delay();
        ctx.counter++;
        cv.notifyOne();
        cv.notifyOne();
        thread.join();

        THEN("Thread should wake only once and increment counter")
        {
            REQUIRE(ctx.counter == 2);
        }
    }
}
