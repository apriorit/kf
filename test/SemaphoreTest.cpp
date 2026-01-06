#include "pch.h"
#include <kf/Semaphore.h>

SCENARIO("kf::Semaphore")
{
    constexpr LARGE_INTEGER kOneMillisecond{ .QuadPart = -10'000LL }; // 1ms
    constexpr LARGE_INTEGER kZeroTimeout{};

    GIVEN("A semaphore with count = 1 and limit = 1")
    {
        kf::Semaphore sem(1, 1);

        WHEN("Trying to acquire should succeed immediately")
        {
            THEN("Thread doesn't wait")
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
            }
        }

        WHEN("Acquiring twice should deplete the count and then timeout")
        {
            REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));

            THEN("Second wait times out")
            {
                REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));
            }
        }

        WHEN("After depleting, release allows acquiring again")
        {
            REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
            REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));

            sem.release();

            THEN("Wait succeeds after release")
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
            }
        }
    }

    GIVEN("A semaphore with count = 0 and limit = 1")
    {
        kf::Semaphore sem(0, 1);

        WHEN("Trying to acquire should timeout")
        {
            THEN("Wait fails with timeout")
            {
                REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));
            }
        }

        WHEN("Release makes next acquire succeed and then timeout again")
        {
            sem.release();

            THEN("First wait succeeds immediately and subsequent wait times out")
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
                REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));
            }
        }
    }

    GIVEN("A semaphore with count = 1000 and limit = 1000")
    {
        kf::Semaphore sem(1000, 1000);

        WHEN("Acquiring up to count succeeds, extra wait times out")
        {
            for (int i = 0; i < 1000; ++i)
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
            }

            THEN("Next wait times out")
            {
                REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));
            }
        }

        WHEN("Releasing after depletion allows acquire to succeed")
        {
            for (int i = 0; i < 1000; ++i)
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
            }
            REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));

            sem.release();

            THEN("Wait succeeds after release")
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
            }
        }
    }

    GIVEN("A semaphore with count = 0, limit = 2 and multi-adjust release")
    {
        kf::Semaphore sem(0, 2);

        WHEN("Release with adjustment 2")
        {
            sem.release(2);

            THEN("Two immediate acquires succeed, third times out")
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
                REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));
            }
        }

        WHEN("Multiple single releases allow multiple acquires")
        {
            sem.release();
            sem.release();

            THEN("Two immediate acquires succeed, next fails")
            {
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
                REQUIRE(STATUS_SUCCESS == sem.wait(&kZeroTimeout));
                REQUIRE(STATUS_TIMEOUT == sem.wait(&kOneMillisecond));
            }
        }
    }
}