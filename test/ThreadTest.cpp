#include "pch.h"
#include <kf/Thread.h>

namespace
{
    struct ThreadContext
    {
        bool started = false;
    };

    void threadProc(void* context)
    {
        static_cast<ThreadContext*>(context)->started = true;
        PsTerminateSystemThread(STATUS_SUCCESS);
    }

    struct TestObject
    {
        ThreadContext m_threadContext;
        
        NTSTATUS run()
        {
            m_threadContext.started = true;
            return STATUS_SUCCESS;
        }
    };
}

SCENARIO("kf::Thread basic lifecycle")
{
    GIVEN("A default constructed thread")
    {
        kf::Thread thread;

        WHEN("join() is called without start()")
        {
            thread.join();

            THEN("it should be a no-op")
            {
                REQUIRE(true);
            }
        }

        WHEN("start() is used with threadProc")
        {
            ThreadContext context;

            const NTSTATUS status = thread.start(&threadProc, &context);

            THEN("start() should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("join() waits for completion")
            {
                thread.join();
                REQUIRE(context.started);
            }
        }

        WHEN("start() is used with lambda")
        {
            ThreadContext context;

            NTSTATUS status = thread.start([](void* context)
                {
                    static_cast<ThreadContext*>(context)->started = true;
                    PsTerminateSystemThread(STATUS_SUCCESS);
                }, 
                &context);

            THEN("start() should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            thread.join();

            THEN("lambda should run")
            {
                REQUIRE(context.started);
            }
        }

        WHEN("start() is used with object member routine")
        {
            TestObject obj;

            const NTSTATUS status = thread.start<&TestObject::run>(&obj);

            THEN("start() should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            thread.join();

            THEN("member routine should run")
            {
                REQUIRE(obj.m_threadContext.started);
            }
        }

        WHEN("calling join twice after start")
        {
            REQUIRE_NT_SUCCESS(thread.start([](void*) { PsTerminateSystemThread(STATUS_SUCCESS); }, nullptr));

            thread.join();
            thread.join();

            THEN("it's safe to call join() multiple times")
            {
            }
        }

        WHEN("thread object is destroyed while running")
        {
            ThreadContext context;

            {
                kf::Thread local;
                REQUIRE_NT_SUCCESS(local.start(&threadProc, &context));
            }

            THEN("destructor should join and thread should have run")
            {
                REQUIRE(context.started);
            }
        }

        WHEN("starting a thread with a long working function")
        {
            ThreadContext context;

            REQUIRE_NT_SUCCESS(thread.start([](void* context)
                {
                    LARGE_INTEGER oneMillisecond{ .QuadPart = -10'000LL }; // 1ms
                    KeDelayExecutionThread(KernelMode, false, &oneMillisecond);

                    static_cast<ThreadContext*>(context)->started = true;
                    PsTerminateSystemThread(STATUS_SUCCESS);
                }, 
                &context));

            thread.join();

            THEN("join() waits for completion")
            {
                thread.join();
                REQUIRE(context.started);
            }
        }
    }
}

SCENARIO("kf::Thread move constructor and move assignment")
{
    GIVEN("A started thread")
    {
        ThreadContext context;
        kf::Thread thread1;
        REQUIRE_NT_SUCCESS(thread1.start(&threadProc, &context));

        WHEN("Thread is move-constructed")
        {
            kf::Thread thread2(std::move(thread1));

            THEN("moved-to object should be able to join")
            {
                thread2.join();
                REQUIRE(context.started);
            }

            THEN("moved-from object join() should be safe")
            {
                thread1.join();
                REQUIRE(context.started);
            }
        }
    }

    GIVEN("Two threads where left-hand side already owns a running thread")
    {
        ThreadContext context1;
        ThreadContext context2;

        kf::Thread thread1;
        kf::Thread thread2;

        REQUIRE_NT_SUCCESS(thread1.start(&threadProc, &context1));
        REQUIRE_NT_SUCCESS(thread2.start(&threadProc, &context2));

        WHEN("Move-assigning thread1 = std::move(thread2)")
        {
            thread1 = std::move(thread2);

            THEN("thread1 should join the thread it now owns")
            {
                thread1.join();
                REQUIRE(context2.started);
            }

            THEN("thread2 should be in a valid empty state")
            {
                thread2.join();
                REQUIRE(true);
            }

            THEN("the original thread from thread1 should have executed")
            {
                REQUIRE(context1.started);
            }
        }
    }
}
