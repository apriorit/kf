#include "pch.h"
#include <kf/algorithm/Algorithm.h>

SCENARIO("Algorithm kf::binary_search_it")
{
    GIVEN("A sorted array of int values")
    {
        constexpr int kSize = 4;
        constexpr std::array kArr = { 111, 222, 333, 444 };

        WHEN("Searching for existing value")
        {
            THEN("Returns iterator to the element")
            {
                auto it = kf::binary_search_it(kArr.begin(), kArr.end(), kArr[1]);
                REQUIRE(it != kArr.end());
                REQUIRE(*it == kArr[1]);
            }
        }

        WHEN("Searching for non-existing value")
        {
            constexpr int kNotInArray = 555;

            THEN("Returns end iterator")
            {
                auto it = kf::binary_search_it(kArr.begin(), kArr.end(), kNotInArray);
                REQUIRE(it == kArr.end());
            }
        }

        WHEN("Searching for the first element")
        {
            THEN("Returns iterator to beginning")
            {
                auto it = kf::binary_search_it(kArr.begin(), kArr.end(), kArr[0]);
                REQUIRE(it == kArr.begin());
                REQUIRE(*it == kArr[0]);
            }
        }

        WHEN("Searching for the last element")
        {
            THEN("Returns iterator to the end - 1")
            {
                auto it = kf::binary_search_it(kArr.begin(), kArr.end(), kArr[kSize - 1]);
                REQUIRE(it == kArr.end() - 1);
                REQUIRE(*it == kArr[kSize - 1]);
            }
        }

        WHEN("Searching in empty vector")
        {
            THEN("Returns iterator to the end")
            {
                constexpr std::array<int, 0> empty;
                auto it = kf::binary_search_it(empty.begin(), empty.end(), kArr[2]);
                REQUIRE(it == empty.end());
            }
        }
    }
}
