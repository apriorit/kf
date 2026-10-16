#include "pch.h"
#include <kf/ThreadPool.h>

namespace
{
    struct ThreadPoolContext
    {
        std::atomic<int> completed = 0;
    };

    void threadPoolProc(void* context)
    {
        ++static_cast<ThreadPoolContext*>(context)->completed;
        PsTerminateSystemThread(STATUS_SUCCESS);
    }

    struct ThreadPoolTestObject
    {
        ThreadPoolContext context;

        NTSTATUS run()
        {
            ++context.completed;
            return STATUS_SUCCESS;
        }
    };
}

static_assert(std::is_nothrow_constructible<kf::ThreadPool<>, int>::value, "Construction from a thread count must not throw");
static_assert(!std::is_copy_constructible<kf::ThreadPool<>>::value, "Thread pools must not be copy constructed");
static_assert(!std::is_copy_assignable<kf::ThreadPool<>>::value, "Thread pools must not be copy assigned");
static_assert(std::is_nothrow_move_constructible<kf::ThreadPool<>>::value, "Thread pools must support non-throwing move construction");
static_assert(std::is_nothrow_move_assignable<kf::ThreadPool<>>::value, "Thread pools must support non-throwing move assignment");
static_assert(std::is_nothrow_destructible<kf::ThreadPool<>>::value, "Thread pool destruction must not throw");

SCENARIO("kf::ThreadPool starts worker routines")
{
    GIVEN("A pool configured for three threads and a shared context")
    {
        ThreadPoolContext context;
        kf::ThreadPool<> pool(3);

        WHEN("The pool is started with a free function")
        {
            const NTSTATUS status = pool.start(&threadPoolProc, &context);
            pool.join();

            THEN("Starting the pool succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("Every worker executes the routine with the supplied context")
            {
                REQUIRE(context.completed == 3);
            }
        }
    }

    GIVEN("A pool configured for three threads and an object with a member routine")
    {
        ThreadPoolTestObject object;
        kf::ThreadPool<> pool(3);

        WHEN("The pool is started with the member routine")
        {
            const NTSTATUS status = pool.start<&ThreadPoolTestObject::run>(&object);
            pool.join();

            THEN("Starting the pool succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("Every worker executes the member routine on the supplied object")
            {
                REQUIRE(object.context.completed == 3);
            }
        }
    }
}

SCENARIO("kf::ThreadPool respects the configured thread count")
{
    GIVEN("A pool configured for zero threads")
    {
        ThreadPoolContext context;
        kf::ThreadPool<> pool(0);

        WHEN("The pool is started and joined")
        {
            const NTSTATUS status = pool.start(&threadPoolProc, &context);
            pool.join();

            THEN("Starting the empty pool succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("No worker executes the routine")
            {
                REQUIRE(context.completed == 0);
            }
        }
    }

    GIVEN("A pool configured for one thread")
    {
        ThreadPoolContext context;
        kf::ThreadPool<> pool(1);

        WHEN("The pool is started and joined")
        {
            pool.start(&threadPoolProc, &context);
            pool.join();

            THEN("Exactly one worker executes the routine")
            {
                REQUIRE(context.completed == 1);
            }
        }
    }

    GIVEN("A pool configured at its custom maximum of three threads")
    {
        ThreadPoolContext context;
        kf::ThreadPool<3> pool(3);

        WHEN("The pool is started and joined")
        {
            pool.start(&threadPoolProc, &context);
            pool.join();

            THEN("Exactly three workers execute the routine")
            {
                REQUIRE(context.completed == 3);
            }
        }
    }

    GIVEN("A pool configured for five threads with a custom maximum of three")
    {
        ThreadPoolContext context;
        kf::ThreadPool<3> pool(5);

        WHEN("The pool is started and joined")
        {
            pool.start(&threadPoolProc, &context);
            pool.join();

            THEN("The worker count is limited to the custom maximum")
            {
                REQUIRE(context.completed == 3);
            }
        }
    }
}

SCENARIO("kf::ThreadPool joins workers")
{
    GIVEN("A pool that has not been started")
    {
        kf::ThreadPool<> pool(3);

        WHEN("The pool is joined")
        {
            pool.join();

            THEN("Joining returns without requiring any workers")
            {
                REQUIRE(true);
            }
        }
    }

    GIVEN("A started pool of three threads")
    {
        ThreadPoolContext context;
        kf::ThreadPool<> pool(3);
        pool.start(&threadPoolProc, &context);

        WHEN("The pool is joined")
        {
            pool.join();

            THEN("All workers have completed before join returns")
            {
                REQUIRE(context.completed == 3);
            }
        }

        WHEN("The pool is joined twice")
        {
            pool.join();
            pool.join();

            THEN("Joining again leaves the completed worker count unchanged")
            {
                REQUIRE(context.completed == 3);
            }
        }

        WHEN("The pool is joined and started again")
        {
            pool.join();
            pool.start(&threadPoolProc, &context);
            pool.join();

            THEN("The workers execute the routine again")
            {
                REQUIRE(context.completed == 6);
            }
        }
    }

    GIVEN("A context that outlives its pool")
    {
        ThreadPoolContext context;

        WHEN("A started pool is destroyed without an explicit join")
        {
            {
                kf::ThreadPool<> pool(3);
                pool.start(&threadPoolProc, &context);
            }

            THEN("All workers have completed before destruction returns")
            {
                REQUIRE(context.completed == 3);
            }
        }
    }
}

SCENARIO("kf::ThreadPool transfers worker ownership")
{
    GIVEN("A started pool of three threads")
    {
        ThreadPoolContext context;
        kf::ThreadPool<> source(3);
        source.start(&threadPoolProc, &context);

        WHEN("The pool is move-constructed and the destination is joined")
        {
            kf::ThreadPool<> destination(std::move(source));
            destination.join();

            THEN("The destination waits for all transferred workers")
            {
                REQUIRE(context.completed == 3);
            }
        }

        WHEN("The pool is move-constructed and the source is joined")
        {
            kf::ThreadPool<> destination(std::move(source));
            source.join();
            destination.join();

            THEN("Joining the moved-from pool does not affect the transferred workers")
            {
                REQUIRE(context.completed == 3);
            }
        }

        WHEN("The pool is move-assigned to an unstarted pool with a different count")
        {
            kf::ThreadPool<> destination(1);
            destination = std::move(source);
            destination.join();

            THEN("The destination waits for the source's full worker count")
            {
                REQUIRE(context.completed == 3);
            }
        }
    }

    GIVEN("Two started pools with different thread counts")
    {
        ThreadPoolContext sourceContext;
        ThreadPoolContext destinationContext;
        kf::ThreadPool<> source(3);
        kf::ThreadPool<> destination(1);
        source.start(&threadPoolProc, &sourceContext);
        destination.start(&threadPoolProc, &destinationContext);

        WHEN("The source is move-assigned to the destination")
        {
            destination = std::move(source);

            THEN("The destination's previous worker has completed")
            {
                REQUIRE(destinationContext.completed == 1);
            }

            THEN("The destination can join all transferred workers")
            {
                destination.join();
                REQUIRE(sourceContext.completed == 3);
            }

            THEN("The moved-from pool can be joined safely")
            {
                source.join();
                REQUIRE(true);
            }
        }
    }
}
