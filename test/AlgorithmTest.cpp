#include "pch.h"
#include <kf/algorithm/Algorithm.h>

SCENARIO("Algorithm kf::binary_search_it")
{
    GIVEN("A sorted array of int values")
    {
        int first = 111;
        int second = 222;
        int third = 333;
        int last = 444;

        int notInArray = 555;

        constexpr int size = 4;
        std::array<int, size> arr = { first, second, third, last };

        WHEN("Searching for existing value")
        {
            THEN("Returns iterator to the element")
            {
                auto it = kf::binary_search_it(arr.begin(), arr.end(), second);
                REQUIRE(it != arr.end());
                REQUIRE(*it == second);
            }
        }

        WHEN("Searching for non-existing value")
        {
            THEN("Returns end iterator")
            {
                auto it = kf::binary_search_it(arr.begin(), arr.end(), notInArray);
                REQUIRE(it == arr.end());
            }
        }

        WHEN("Searching for the first element")
        {
            THEN("Returns iterator to beginning")
            {
                auto it = kf::binary_search_it(arr.begin(), arr.end(), first);
                REQUIRE(it == arr.begin());
                REQUIRE(*it == first);
            }
        }

        WHEN("Searching for the last element")
        {
            THEN("Returns iterator to the end - 1")
            {
                auto it = kf::binary_search_it(arr.begin(), arr.end(), last);
                REQUIRE(it == arr.end() - 1);
                REQUIRE(*it == last);
            }
        }

        WHEN("Searching in empty vector")
        {
            THEN("Returns iterator to the end")
            {
                std::array<int, 0> empty;
                auto it = kf::binary_search_it(empty.begin(), empty.end(), third);
                REQUIRE(it == empty.end());
            }
        }
    }
}
