#include "pch.h"
#include <kf/Singleton.h>

class TestSingleton : public kf::Singleton<TestSingleton>
{
public:
    int value = 0;
};

SCENARIO("Testing Singleton functionality")
{
    WHEN("A TestSingleton object was not created yet")
    {
        THEN("getInstance returns nullptr")
        {
            REQUIRE(TestSingleton::getInstance() == nullptr);
        }
    }

    WHEN("A TestSingleton object is created")
    {
        TestSingleton singleton;

        THEN("getInstance returns pointer to the created instance")
        {
            REQUIRE(TestSingleton::getInstance() == &singleton);
        }

        THEN("Singleton instance member can be accessed")
        {
            singleton.value = 42;
            REQUIRE(TestSingleton::getInstance()->value == 42);
        }
    }

    WHEN("The TestSingleton object is destroyed")
    {
        {
            TestSingleton singleton;
            REQUIRE(TestSingleton::getInstance() == &singleton);
        }

        THEN("getInstance returns nullptr after destruction")
        {
            REQUIRE(TestSingleton::getInstance() == nullptr);
        }
    }
}
