#include "pch.h"
#include <kf/Guard.h>

struct TestResource
{
    int m_value = 0;
};

static int g_closeCallCount = 0;
static TestResource* g_lastClosedResource = nullptr;

void closeFn(TestResource* res)
{
    ++g_closeCallCount;
    g_lastClosedResource = res;
}

SCENARIO("Testing kf::Guard::Guard basic functionality")
{
    using TestGuard = kf::Guard::Guard< TestResource*, nullptr, decltype(&closeFn), &closeFn>;

    GIVEN("A default constructed Guard")
    {
        TestGuard guard;
        g_closeCallCount = 0;
        g_lastClosedResource = nullptr;

        THEN("Its value is nullptr and close function is not called on destruction")
        {
            REQUIRE(static_cast<TestResource*>(guard) == nullptr);
            REQUIRE(g_closeCallCount == 0);
        }

        WHEN("reset is called with nullptr explicitly")
        {
            guard.reset(nullptr);
            THEN("Close function is not called")
            {
                REQUIRE(g_closeCallCount == 0);
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("A Guard constructed with a valid resource pointer")
    {
        int value = 123;
        TestResource resource(value);
        TestGuard guard(&resource);
        g_closeCallCount = 0;
        g_lastClosedResource = nullptr;

        THEN("Value is stored correctly")
        {
            REQUIRE(guard.get() == &resource);
            REQUIRE(guard.get()->m_value == value);
        }

        WHEN("Guard is reset with no arguments")
        {
            guard.reset();

            THEN("Close function is called once with the resource")
            {
                REQUIRE(g_closeCallCount == 1);
                REQUIRE(g_lastClosedResource == &resource);
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("A Guard constructed with a valid resource pointer")
    {
        TestResource resource(123);
        TestGuard guard(&resource);
        g_closeCallCount = 0;
        g_lastClosedResource = nullptr;

        WHEN("Guard is released")
        {
            TestResource* released = guard.release();

            THEN("Released value is the resource pointer")
            {
                REQUIRE(released == &resource);
            }

            THEN("Guard now holds nullptr")
            {
                REQUIRE(guard.get() == nullptr);
            }

            THEN("Close function was not called on release")
            {
                REQUIRE(g_closeCallCount == 0);
            }
        }
    }

    GIVEN("A Guard constructed with a valid resource pointer")
    {
        int value = 123;
        TestResource resource(value);
        TestGuard guard(&resource);
        g_closeCallCount = 0;
        g_lastClosedResource = nullptr;

        WHEN("Guard is move-constructed from another Guard")
        {
            TestGuard otherGuard(std::move(guard));

            THEN("The second Guard owns the resource")
            {
                REQUIRE(otherGuard.get() == &resource);
                REQUIRE(otherGuard.get()->m_value == value);
            }

            THEN("The original guard holds nullptr and no closeFn was called for the resource")
            {
                REQUIRE(g_closeCallCount == 0);
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("A Guard constructed with a valid resource pointer")
    {
        int value = 123;
        TestResource resource(value);
        TestGuard guard(&resource);
        g_closeCallCount = 0;
        g_lastClosedResource = nullptr;

        WHEN("Guard is move-assigned from another Guard")
        {
            int otherValue = 456;
            TestResource otherResource(otherValue);
            TestGuard guard2(&otherResource);

            guard2 = std::move(guard);

            THEN("The second guard now owns the first resource")
            {
                REQUIRE(guard2.get() == &resource);
                REQUIRE(guard2.get()->m_value == value);
            }

            THEN("The original guard holds nullptr and closeFn was called for other resource")
            {
                REQUIRE(g_lastClosedResource == &otherResource);
                REQUIRE(g_closeCallCount == 1);
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("A Guard constructed with a valid resource pointer")
    {
        TestResource resource(123);
        g_closeCallCount = 0;
        g_lastClosedResource = nullptr;

        WHEN("Guard is destroyed")
        {
            {
                TestGuard guard(&resource);
            }

            THEN("Close function is called on destruction")
            {
                REQUIRE(g_closeCallCount == 1);
                REQUIRE(g_lastClosedResource == &resource);
            }
        }
    }
}
