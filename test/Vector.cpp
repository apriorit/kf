#include "pch.h"
#include <kf/stl/vector>

SCENARIO("vector size and capacity")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("do nothing")
        {
            THEN("vector size is 0, capacity is 0")
            {
                REQUIRE(v.size() == 0);
                REQUIRE(v.capacity() == 0);
            }
        }

        WHEN("push_back 1 element with value 100")
        {
            REQUIRE_NT_SUCCESS(v.push_back(100));

            THEN("vector size is 1, capacity is >= 1")
            {
                REQUIRE(v.size() == 1);
                REQUIRE(v.capacity() >= 1);
            }
        }

        WHEN("assign 1000 elements with value 12345")
        {
            REQUIRE_NT_SUCCESS(v.assign(1000, 12345));

            THEN("vector size is 1000, capacity is >= 1000")
            {
                REQUIRE(v.size() == 1000);
                REQUIRE(v.capacity() >= 1000);
            }
        }

        WHEN("resize to 1000")
        {
            REQUIRE_NT_SUCCESS(v.resize(1000));

            THEN("vector size is 1000, capacity is >= 1000")
            {
                REQUIRE(v.size() == 1000);
                REQUIRE(v.capacity() >= 1000);
            }
        }
    }

    GIVEN("vector with 1000 elements")
    {
        kf::vector<int, PagedPool> v;
        REQUIRE_NT_SUCCESS(v.assign(1000, 12345));

        WHEN("resize to 1")
        {
            REQUIRE_NT_SUCCESS(v.resize(1));

            THEN("vector size is 1, capacity is >= 1000")
            {
                REQUIRE(v.size() == 1);
                REQUIRE(v.capacity() >= 1000);
            }
        }

        WHEN("resize to 1 and shrink_to_fit")
        {
            REQUIRE_NT_SUCCESS(v.resize(1));
            REQUIRE_NT_SUCCESS(v.shrink_to_fit());

            THEN("vector size is 1, capacity is 1")
            {
                REQUIRE(v.size() == 1);
                REQUIRE(v.capacity() == 1);
            }
        }

        WHEN("clear")
        {
            v.clear();

            THEN("vector size is 0, capacity is >= 1000")
            {
                REQUIRE(v.size() == 0);
                REQUIRE(v.capacity() >= 1000);
            }
        }
    }
}

SCENARIO("vector iterators")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("do nothing")
        {
            THEN("(c)(r)begin == (c)(r)end")
            {
                REQUIRE(v.begin() == v.end());
                REQUIRE(v.cbegin() == v.cend());
                REQUIRE(v.rbegin() == v.rend());
                REQUIRE(v.crbegin() == v.crend());
            }
        }

        WHEN("push_back 1 element with value 100")
        {
            REQUIRE_NT_SUCCESS(v.push_back(100));

            THEN("difference between (c)(r)begin and (c)(r)end is 1")
            {
                REQUIRE(v.end() - v.begin() == 1);
                REQUIRE(v.cend() - v.cbegin() == 1);
                REQUIRE(v.rend() - v.rbegin() == 1);
                REQUIRE(v.crend() - v.crbegin() == 1);
            }
        }
    }
}

SCENARIO("vector at and [] access")
{
    GIVEN("empty vector")
    {
        kf::vector<int, PagedPool> v;

        WHEN("do nothing")
        {
            THEN("at returns nullopt")
            {
                REQUIRE(v.at(0) == std::nullopt);
            }
        }

        WHEN("push_back 1 element with value 100")
        {
            REQUIRE_NT_SUCCESS(v.push_back(100));

            THEN("value of element with index 0 is 100")
            {
                REQUIRE(v[0] == 100);
                REQUIRE(v.at(0).has_value());
                REQUIRE(*v.at(0) == 100);
            }
        }
    }
}