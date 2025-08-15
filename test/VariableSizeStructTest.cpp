#include "pch.h"
#include <kf/VariableSizeStruct.h>

static int g_ctorCount = 0;
static int g_dtorCount = 0;

struct TestStruct
{
    int value1;
    int value2;

    TestStruct(int a, int b) : value1(a), value2(b)
    { 
        ++g_ctorCount;
    }

    ~TestStruct()
    {
        ++g_dtorCount;
    }
};

SCENARIO("kf::VariableSizeStruct")
{
    GIVEN("A default constructed VariableSizeStruct")
    {
        g_ctorCount = 0;
        g_dtorCount = 0;
        kf::VariableSizeStruct<TestStruct, PagedPool> var;

        THEN("It should not allocate memory initially")
        {
            REQUIRE(var.get() == nullptr);
            REQUIRE(g_ctorCount == 0);
            REQUIRE(g_dtorCount == 0);
        }

        WHEN("emplace is called")
        {
            auto status = var.emplace(sizeof(TestStruct), 123, 456);

            THEN("It should return STATUS_SUCCESS and construct object")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(var.get() != nullptr);
                REQUIRE(var->value1 == 123);
                REQUIRE(var->value2 == 456);
                REQUIRE(g_ctorCount == 1);
                REQUIRE(g_dtorCount == 0);
            }
        }
    }

    GIVEN("A VariableSizeStruct created with constructor and arguments")
    {
        g_ctorCount = 0;
        g_dtorCount = 0;

        {
            kf::VariableSizeStruct<TestStruct, PagedPool> var(sizeof(TestStruct), 123, 456);
            THEN("It should allocate memory and construct object")
            {
                REQUIRE(var.get() != nullptr);
                REQUIRE(var->value1 == 123);
                REQUIRE(var->value2 == 456);
                REQUIRE(g_ctorCount == 1);
                REQUIRE(g_dtorCount == 0);
            }
        }

        THEN("It should destroy object on scope exit")
        {
            REQUIRE(g_dtorCount == 1);
        }
    }

    GIVEN("A VariableSizeStruct constructed with an object")
    {
        g_ctorCount = 0;
        g_dtorCount = 0;
        kf::VariableSizeStruct<TestStruct, PagedPool> var(sizeof(TestStruct), 123, 456);

        WHEN("free is called")
        {
            var.free();
            THEN("It should destroy object and free memory")
            {
                REQUIRE(var.get() == nullptr);
                REQUIRE(g_dtorCount == 1);
            }
        }
    }

    GIVEN("A VariableSizeStruct with multiple emplace calls")
    {
        g_ctorCount = 0;
        g_dtorCount = 0;

        kf::VariableSizeStruct<TestStruct, PagedPool> var;

        WHEN("emplace is called twice")
        {
            var.emplace(sizeof(TestStruct), 111, 222);
            var.emplace(sizeof(TestStruct), 333, 444);

            THEN("Old object should be destroyed before constructing new one")
            {
                REQUIRE(g_ctorCount == 2);
                REQUIRE(g_dtorCount == 1);
                REQUIRE(var->value1 == 333);
                REQUIRE(var->value2 == 444);
            }
        }
    }
}
