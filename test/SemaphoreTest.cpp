#include "pch.h"
#include <kf/Semaphore.h>
#include <kf/Thread.h>

namespace
{
    constexpr int kOneMillisecond = 1000;

    struct Context
    {
        kf::Semaphore* semaphore = nullptr;
        NTSTATUS status = STATUS_WAIT_1;
    };

    void delay()
    {
        LARGE_INTEGER timeout;
        timeout.QuadPart = -kOneMillisecond;
        KeDelayExecutionThread(KernelMode, false, &timeout);
    }
}

SCENARIO("kf::Semaphore")
{
    GIVEN("A semaphore with count = 1 and limit = 1")
    {
        kf::Semaphore sem(1, 1);
        Context ctx{ &sem };
        kf::Thread thread;

        WHEN("Trying to acquire should succeed immediately")
        {
            thread.start([](void* context) {
                auto ctx = static_cast<Context*>(context);
                ctx->status = ctx->semaphore->wait();
                }, &ctx);

            thread.join();

            THEN("Thread doesn't wait")
            {
                REQUIRE(ctx.status == STATUS_SUCCESS);
            }
        }
    }

    GIVEN("A semaphore with count = 0 and limit = 1")
    {
        kf::Semaphore sem(0, 1);

        WHEN("Threads wait with 0")
        {
            kf::Thread thread;
            Context ctx{ &sem };

            thread.start([](void* context) {
                auto ctx = static_cast<Context*>(context);
                LARGE_INTEGER timeout{ 0 };
                ctx->status = ctx->semaphore->wait(&timeout);
                }, &ctx);

            thread.join();

            THEN("Thread should be timed out")
            {
                REQUIRE(ctx.status == STATUS_TIMEOUT);
            }
        }

        WHEN("Threads wait with 100 milliseconds")
        {
            kf::Thread thread;
            Context ctx{ &sem };

            thread.start([](void* context) {
                auto ctx = static_cast<Context*>(context);
                LARGE_INTEGER timeout;
                timeout.QuadPart = kOneMillisecond * -100;
                ctx->status = ctx->semaphore->wait(&timeout);
                }, &ctx);

            delay();

            THEN("Thread should be waiting")
            {
                REQUIRE(ctx.status == STATUS_WAIT_1);
            }

            thread.join();

            THEN("Thread should be timed out after 100 milliseconds")
            {
                REQUIRE(ctx.status == STATUS_TIMEOUT);
            }
        }
    }

    GIVEN("Semaphore with count = 0 and limit = 2")
    {
        kf::Semaphore sem(0, 2);
        Context ctx1{ &sem }, ctx2{ &sem };
        kf::Thread thread1, thread2;

        thread1.start([](void* context) {
            auto ctx = static_cast<Context*>(context);
            ctx->status = ctx->semaphore->wait();
            }, &ctx1);

        thread2.start([](void* context) {
            auto ctx = static_cast<Context*>(context);
            ctx->status = ctx->semaphore->wait();
            }, &ctx2);

        delay();

        THEN("Both threads are waiting")
        {
            REQUIRE(ctx1.status == STATUS_WAIT_1);
            REQUIRE(ctx2.status == STATUS_WAIT_1);
        }

        WHEN("We release semaphore with 2")
        {
            sem.release(2);
            thread1.join();
            thread2.join();

            THEN("Both threads should stop waiting")
            {
                REQUIRE(ctx1.status == STATUS_SUCCESS);
                REQUIRE(ctx2.status == STATUS_SUCCESS);
            }
        }
    }

    GIVEN("Semaphore with count = 2 and limit = 2")
    {
        kf::Semaphore sem(2, 2);
        Context ctx{ &sem };
        kf::Thread thread;

        WHEN("Semaphore released over limit twice")
        {
            sem.release();
            sem.release();

            THEN("No crash occurs")
            {
            }
        }
    }

    GIVEN("Semaphore with count = 1 and limit = 2")
    {
        kf::Semaphore sem(1, 2);
        Context ctx1{ &sem }, ctx2{ &sem }, ctx3{ &sem };
        kf::Thread thread1, thread2, thread3;

        WHEN("Semaphore is released twice, exceeding the limit")
        {
            sem.release();
            sem.release();

            THEN("First two threads should succeed, third should wait")
            {
                thread1.start([](void* context) {
                    auto ctx = static_cast<Context*>(context);
                    ctx->status = ctx->semaphore->wait();
                    }, &ctx1);

                thread2.start([](void* context) {
                    auto ctx = static_cast<Context*>(context);
                    ctx->status = ctx->semaphore->wait();
                    }, &ctx2);

                thread3.start([](void* context) {
                    auto ctx = static_cast<Context*>(context);
                    ctx->status = ctx->semaphore->wait();
                    }, &ctx3);

                delay();

                int waitCount = (ctx1.status != STATUS_SUCCESS) + (ctx2.status != STATUS_SUCCESS) + (ctx3.status != STATUS_SUCCESS);
                REQUIRE(waitCount == 1);

                sem.release();
                thread1.join();
                thread2.join();
                thread3.join();

                THEN("Third thread should succeed after release")
                {
                    REQUIRE(ctx3.status == STATUS_SUCCESS);
                }
            }
        }
    }
}