#include "pch.h"
#include <kf/algorithm/Algorithm.h>
#include <kf/USimpleString.h>
#include <kf/stl/vector>

SCENARIO("Algorithm binary_search_it")
{
    GIVEN("A sorted vector of USimpleString values")
    {
        kf::USimpleString first (L"AAA");
        kf::USimpleString second(L"BBB");
        kf::USimpleString third (L"CCC");
        kf::USimpleString last  (L"DDD");

        kf::USimpleString notInVec(L"FFF");

        kf::vector<kf::USimpleString, NonPagedPoolNx> vec;
        vec.push_back(first);
        vec.push_back(second);
        vec.push_back(third);
        vec.push_back(last); 

        WHEN("Searching for existing string")
        {
            THEN("Returns iterator to the element")
            {
                auto it = kf::binary_search_it(vec.begin(), vec.end(), second);
                REQUIRE(it != vec.end());
                REQUIRE(it->equals(second));
            }
        }

        WHEN("Searching for non-existing string")
        {
            THEN("Returns end iterator")
            {
                auto it = binary_search_it(vec.begin(), vec.end(), notInVec);
                REQUIRE(it == vec.end());
            }
        }

        WHEN("Searching for the first element")
        {
            THEN("Returns iterator to beginning")
            {
                auto it = binary_search_it(vec.begin(), vec.end(), first);
                REQUIRE(it == vec.begin());
                REQUIRE(it->equals(first));
            }
        }

        WHEN("Searching for the last element")
        {
            THEN("Returns iterator to the end - 1")
            {
                auto it = binary_search_it(vec.begin(), vec.end(), last);
                REQUIRE(it == vec.end() - 1);
                REQUIRE(it->equals(last));
            }
        }

        WHEN("Searching in empty vector")
        {
            THEN("Returns iterator to the end")
            {
                kf::vector<kf::USimpleString, NonPagedPoolNx> empty;
                auto it = binary_search_it(empty.begin(), empty.end(), third);
                REQUIRE(it == empty.end());
            }
        }
    }
}
