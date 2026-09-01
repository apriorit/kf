#include "pch.h"
#include <kf/stl/map>

SCENARIO("Map test: insert and find integers")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    THEN("Map is initialized")
    {
        REQUIRE_NT_SUCCESS(map.initialize());
    }

    THEN("Map is empty")
    {
        REQUIRE(map.empty());
        REQUIRE(map.size() == 0);
    }

    const int kIterations = 5; // Number of iterations to insert
    const int kKeyMultiplier = 2; // Multiplier for keys to ensure they are not equal to iteration index

    WHEN("Map is populated with integers")
    {
        for (int i = 0; i < kIterations; ++i)
        {
            auto result = map.emplace(i * kKeyMultiplier, i * kIterations);
            REQUIRE(result.has_value());
            REQUIRE(result->second);
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

        THEN("The value for key greater than max key is not found")
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

        THEN("operator[] works with const key")
        {
            const int constKey = 2;
            auto result = map[constKey];
            REQUIRE(result.has_value());
            REQUIRE(result->get() == 2 * kIterations);
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

SCENARIO("Map test: at() method")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    WHEN("Map is populated")
    {
        auto result = map.emplace(1, 10);
        REQUIRE(result.has_value());
        REQUIRE(result->second);

        result = map.emplace(2, 20);
        REQUIRE(result.has_value());
        REQUIRE(result->second);

        THEN("at() returns value for existing key")
        {
            auto value = map.at(1);
            REQUIRE(value.has_value());
            REQUIRE(value->get() == 10);

            auto constValue = const_cast<const decltype(map)*>(&map)->at(2);
            REQUIRE(constValue.has_value());
            REQUIRE(constValue->get() == 20);
        }

        THEN("at() returns empty optional for non-existing key")
        {
            auto value = map.at(99);
            REQUIRE(!value.has_value());

            auto constValue = const_cast<const decltype(map)*>(&map)->at(99);
            REQUIRE(!constValue.has_value());
        }
    }
}

SCENARIO("Map test: insert() methods")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    WHEN("Using insert with value_type")
    {
        auto result = map.insert(std::make_pair(1, 10));
        REQUIRE(result.has_value());
        REQUIRE(result->second); // inserted

        THEN("Element is found")
        {
            REQUIRE(map.contains(1));
            auto it = map.find(1);
            REQUIRE(it != map.end());
            REQUIRE(it->second == 10);
        }

        THEN("Insert same key returns false")
        {
            auto result2 = map.insert(std::make_pair(1, 20));
            REQUIRE(result2.has_value());
            REQUIRE(!result2->second); // not inserted
            REQUIRE(result2->first->second == 10); // original value
        }
    }

    WHEN("Using insert with hint")
    {
        auto result = map.insert(map.end(), std::make_pair(5, 50));
        REQUIRE(result.has_value());
        REQUIRE(map.contains(5));
        REQUIRE(map.find(5)->second == 50);
    }

    WHEN("Using insert with initializer list")
    {
        auto result = map.insert({ {1, 10}, {2, 20}, {3, 30} });
        REQUIRE(result.has_value());
        
        REQUIRE(map.size() == 3);
        REQUIRE(map.contains(1));
        REQUIRE(map.contains(2));
        REQUIRE(map.contains(3));
    }
}

SCENARIO("Map test: count() and equal_range()")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    map.emplace(1, 10);
    map.emplace(2, 20);
    map.emplace(3, 30);

    WHEN("Using count()")
    {
        REQUIRE(map.count(1) == 1);
        REQUIRE(map.count(2) == 1);
        REQUIRE(map.count(99) == 0);
    }

    WHEN("Using equal_range()")
    {
        auto range = map.equal_range(2);
        REQUIRE(range.first != map.end());
        REQUIRE(range.first->first == 2);
        REQUIRE(range.first->second == 20);
        REQUIRE(range.second != range.first);
        
        auto constRange = const_cast<const decltype(map)*>(&map)->equal_range(2);
        REQUIRE(constRange.first != map.end());
        REQUIRE(constRange.first->first == 2);
    }
}

SCENARIO("Map test: reverse iterators")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    map.emplace(1, 10);
    map.emplace(2, 20);
    map.emplace(3, 30);

    WHEN("Using reverse iterators")
    {
        auto rit = map.rbegin();
        REQUIRE(rit != map.rend());
        REQUIRE(rit->first == 3);
        REQUIRE(rit->second == 30);

        ++rit;
        REQUIRE(rit->first == 2);
        REQUIRE(rit->second == 20);

        THEN("Const reverse iterators work")
        {
            auto crit = const_cast<const decltype(map)*>(&map)->rbegin();
            REQUIRE(crit != map.rend());
            REQUIRE(crit->first == 3);

            auto crrit = map.crbegin();
            REQUIRE(crrit != map.crend());
            REQUIRE(crrit->first == 3);
        }
    }
}

SCENARIO("Map test: utility methods")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    WHEN("Testing utility methods")
    {
        REQUIRE(map.max_size() > 0);
        
        auto keyComp = map.key_comp();
        REQUIRE(keyComp(1, 2));
        REQUIRE(!keyComp(2, 1));
        
        auto valueComp = map.value_comp();
        REQUIRE(valueComp(std::make_pair(1, 10), std::make_pair(2, 20)));
    }
}

SCENARIO("Map test: try_emplace()")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    WHEN("Using try_emplace")
    {
        auto result = map.try_emplace(1, 10);
        REQUIRE(result.has_value());
        REQUIRE(result->second); // inserted
        REQUIRE(result->first->second == 10);

        THEN("try_emplace with existing key doesn't modify value")
        {
            auto result2 = map.try_emplace(1, 20);
            REQUIRE(result2.has_value());
            REQUIRE(!result2->second); // not inserted
            REQUIRE(result2->first->second == 10); // original value
        }

        THEN("try_emplace with hint works")
        {
            auto result3 = map.try_emplace(map.end(), 2, 20);
            REQUIRE(result3.has_value());
            REQUIRE(map.contains(2));
            REQUIRE(map.find(2)->second == 20);
        }
    }
}

SCENARIO("Map test: insert_or_assign()")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    WHEN("Using insert_or_assign")
    {
        auto result = map.insert_or_assign(1, 10);
        REQUIRE(result.has_value());
        REQUIRE(result->second); // inserted
        REQUIRE(result->first->second == 10);

        THEN("insert_or_assign with existing key modifies value")
        {
            auto result2 = map.insert_or_assign(1, 20);
            REQUIRE(result2.has_value());
            REQUIRE(!result2->second); // not inserted (assigned)
            REQUIRE(result2->first->second == 20); // new value
        }

        THEN("insert_or_assign with hint works")
        {
            auto result3 = map.insert_or_assign(map.end(), 2, 30);
            REQUIRE(result3.has_value());
            REQUIRE(map.contains(2));
            REQUIRE(map.find(2)->second == 30);
        }
    }
}

SCENARIO("Map test: range insert with proper memory allocation")
{
    kf::map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    WHEN("Using range insert with vector")
    {
        std::vector<std::pair<int, int>> data = {
            {1, 10}, {2, 20}, {3, 30}, {4, 40}
        };
        
        auto result = map.insert(data.begin(), data.end());
        REQUIRE(result.has_value());
        
        THEN("All elements are inserted")
        {
            REQUIRE(map.size() == 4);
            REQUIRE(map.contains(1));
            REQUIRE(map.contains(2));
            REQUIRE(map.contains(3));
            REQUIRE(map.contains(4));
            REQUIRE(map.find(1)->second == 10);
            REQUIRE(map.find(2)->second == 20);
            REQUIRE(map.find(3)->second == 30);
            REQUIRE(map.find(4)->second == 40);
        }
    }
    
    WHEN("Using range insert with some existing elements")
    {
        // Pre-populate with some elements
        map.emplace(2, 200);
        map.emplace(4, 400);
        
        std::vector<std::pair<int, int>> data = {
            {1, 10}, {2, 20}, {3, 30}, {4, 40}, {5, 50}
        };
        
        auto result = map.insert(data.begin(), data.end());
        REQUIRE(result.has_value());
        
        THEN("Only new elements are inserted, existing ones unchanged")
        {
            REQUIRE(map.size() == 5);
            REQUIRE(map.find(1)->second == 10);  // new
            REQUIRE(map.find(2)->second == 200); // existing unchanged
            REQUIRE(map.find(3)->second == 30);  // new
            REQUIRE(map.find(4)->second == 400); // existing unchanged
            REQUIRE(map.find(5)->second == 50);  // new
        }
    }
}
