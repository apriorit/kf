#include "pch.h"
#include <kf/Map.h>

SCENARIO("Insert and find integers")
{
    kf::Map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    static const int kIterations = 5; // Number of iterations to insert
    static const int kKeyMultiplier = 2; // Multiplier for keys to ensure they are not equal to iteration index
    WHEN("Map is populated with integers")
    {
        for (int i = 0; i < kIterations; ++i)
        {
            auto result = map.emplace(i * kKeyMultiplier, i * kIterations);
            REQUIRE(result.has_value());
            REQUIRE(result.value().second);
        }

        THEN("It contains proper count of pairs")
        {
            REQUIRE(!map.empty());
            REQUIRE(map.size() == kIterations);
        }

        THEN("All values are found by existing keys")
        {
            for (int i = 0; i < kIterations; i += kKeyMultiplier)
            {
                REQUIRE(map.contains(i * kKeyMultiplier));

                auto value = map.find(i * kKeyMultiplier);
                REQUIRE(value != map.end());
                REQUIRE((*value).first == i * kKeyMultiplier);
                REQUIRE((*value).second == i * kIterations);
            }
        }

        THEN("The value for key greater then max key is not found")
        {
            auto value = map.find(kIterations * kKeyMultiplier + 1);
            REQUIRE(value == map.end());
        }

        THEN("The begin is valid")
        {
            REQUIRE(map.begin()->first == 0);
            REQUIRE(map.begin()->second == 0);
        }

        THEN("The end is valid")
        {
            auto end = map.end();
            --end;
            REQUIRE(end->first == (kIterations - 1) * kKeyMultiplier);
            REQUIRE(end->second == (kIterations - 1) * kIterations);
        }

        THEN("The map can be erased by key")
        {
            auto erasedCount = map.erase(0);
            REQUIRE(erasedCount == 1);

            REQUIRE(map.size() == kIterations - 1);
            REQUIRE(!map.contains(0));

            auto value = map.find(0);
            REQUIRE(value == map.end());
        }

        THEN("The value can be erased by iterator")
        {
            auto it = map.find(2);
            REQUIRE(it != map.end());

            auto erasedIt = map.erase(it);
            REQUIRE(erasedIt != map.end());
            REQUIRE(erasedIt->first == 4);
        }

        THEN("The map can be cleared")
        {
            map.clear();
            REQUIRE(map.empty());
            REQUIRE(map.size() == 0);
        }

        THEN("upper_bound works properly")
        {
            // upper_bound finds next for existing element
            auto it = map.upper_bound(4);
            REQUIRE(it != map.end());
            REQUIRE(it->first == 6);

            // upper_bound finds next for not existing element
            it = map.upper_bound(5);
            REQUIRE(it != map.end());
            REQUIRE(it->first == 6);

            // upper_bound finds first element
            it = map.upper_bound(-1);
            REQUIRE(it != map.end());
            REQUIRE(it->first == 0);

            // upper_bound doesn't find element after the last one
            it = map.upper_bound(9);
            REQUIRE(it == map.end());
        }

        THEN("lower_bound works properly")
        {
            // lower_bound finds existing element
            auto it = map.lower_bound(4);
            REQUIRE(it != map.end());
            REQUIRE(it->first == 4);

            // lower_bound finds next for not existing element
            it = map.lower_bound(5);
            REQUIRE(it != map.end());
            REQUIRE(it->first == 6);

            // lower_bound finds first element
            it = map.lower_bound(-1);
            REQUIRE(it != map.end());
            REQUIRE(it->first == 0);

            // lower_bound finds last element
            it = map.lower_bound(8);
            REQUIRE(it != map.end());
            REQUIRE(it->first == 8);

            // lower_bound doesn't find element after the last one
            it = map.lower_bound(9);
            REQUIRE(it == map.end());
        }
    }
}
