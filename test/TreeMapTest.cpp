#include "pch.h"
#include <kf/TreeMap.h>

using namespace kf;
using Map = kf::TreeMap<int, int, PagedPool>;

namespace
{
    constexpr int KeyToValue(int key)
    {
        return key * 10;
    }

    constexpr int ValueToKey(int value)
    {
        return value / 10;
    }

    constexpr int kMinKey = 1;
    constexpr int kMaxKey = 5;
    constexpr int kMapSize = kMaxKey - kMinKey + 1;
}

SCENARIO("TreeMap: all methods")
{
    Map map;

    GIVEN("empty map")
    {
        WHEN("isEmpty is called")
        {
            auto isEmpty = map.isEmpty();

            THEN("the value is true")
            {
                REQUIRE(isEmpty == true);
            }
        }

        WHEN("size is called")
        {
            auto size = map.size();

            THEN("the size is 0")
            {
                REQUIRE(size == 0);
            }
        }

        WHEN("containsKey is called")
        {
            auto contains = map.containsKey(kMinKey);

            THEN("the value is false")
            {
                REQUIRE(contains == false);
            }
        }

        WHEN("get is called")
        {
            auto value = map.get(kMinKey);

            THEN("the value is nullptr")
            {
                REQUIRE(value == nullptr);
            }
        }

        WHEN("remove is called")
        {
            auto removed = map.remove(kMinKey);

            THEN("the removal should fail")
            {
                REQUIRE(removed == false);
            }
        }

        WHEN("clear is called")
        {
            map.clear();

            THEN("map is still empty")
            {
                REQUIRE(map.isEmpty() == true);
                REQUIRE(map.size() == 0);
            }
        }
    }

    GIVEN("map with elements")
    {
        for (int key = kMinKey; key <= kMaxKey; ++key)
        {
            REQUIRE_NT_SUCCESS(map.put(key, KeyToValue(key)));
        }

        WHEN("isEmpty is called")
        {
            THEN("the value is false")
            {
                REQUIRE(map.isEmpty() == false);
            }
        }

        WHEN("size is called")
        {
            THEN("the size is valid")
            {
                REQUIRE(map.size() == kMapSize);
            }
        }

        WHEN("containsKey is called for existing key")
        {
            auto contains = map.containsKey(kMinKey + 1);

            THEN("the value should be true")
            {
                REQUIRE(contains == true);
            }
        }

        WHEN("containsKey is called for non-existing key")
        {
            auto contains = map.containsKey(kMinKey - 1);

            THEN("the value should be false")
            {
                REQUIRE(contains == false);
            }
        }

        WHEN("get is called for existing key")
        {
            auto value = map.get(kMaxKey / 2);

            THEN("the value should be valid")
            {
                REQUIRE(value != nullptr);
                REQUIRE(*value == KeyToValue(kMaxKey / 2));
            }
        }

        WHEN("get is called for non-existing key")
        {
            auto value = map.get(kMaxKey + 2);

            THEN("the value should be nullptr")
            {
                REQUIRE(value == nullptr);
            }
        }

        WHEN("getByIndex is called for existing index")
        {
            auto value = map.getByIndex(0);

            THEN("the value should be valid")
            {
                REQUIRE(value != nullptr);
                REQUIRE(*value == KeyToValue(kMinKey));
            }
        }

        WHEN("getByIndex is called for non-existing index")
        {
            auto value = map.getByIndex(kMapSize + 1);

            THEN("the value should be nullptr")
            {
                REQUIRE(value == nullptr);
            }
        }

        WHEN("put is called for new key")
        {
            int newKey = kMaxKey + 1;
            int newValue = KeyToValue(newKey);
            auto status = map.put(newKey, newValue);

            THEN("the put should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("the value should be accessible")
            {
                auto value = map.get(newKey);
                REQUIRE(value != nullptr);
                REQUIRE(*value == newValue);
            }

            THEN("the size should increase")
            {
                REQUIRE(map.size() == kMapSize + 1);
            }
        }

        WHEN("put is called for existing key")
        {
            const int newValue = KeyToValue(kMinKey) + 100;
            auto status = map.put(kMinKey, newValue);

            THEN("the put should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("the value should be updated")
            {
                auto value = map.get(kMinKey);
                REQUIRE(value != nullptr);
                REQUIRE(*value == newValue);
            }
        }

        WHEN("remove is called for existing key")
        {
            auto removed = map.remove(kMinKey);

            THEN("the removal should succeed")
            {
                REQUIRE(removed == true);
            }
            
            THEN("the key is no longer in the map")
            {
                auto value = map.get(kMinKey);
                REQUIRE(value == nullptr);
            }

            THEN("the size is reduced")
            {
                REQUIRE(map.size() == kMapSize - 1);
            }
        }

        WHEN("remove is called for non-existing key")
        {
            auto removed = map.remove(kMinKey - 1);

            THEN("the removal should fail")
            {
                REQUIRE(removed == false);
            }

            THEN("the size remains unchanged")
            {
                REQUIRE(map.size() == kMapSize);
            }
        }

        WHEN("clear is called")
        {
            map.clear();

            THEN("map is empty")
            {
                REQUIRE(map.isEmpty() == true);
                REQUIRE(map.size() == 0);
            }
            
            THEN("valid key is no longer accessible")
            {
                REQUIRE(map.get(kMinKey) == nullptr);
            }
        }

        WHEN("iterate over map using getByIndex")
        {
            int count = 0;

            THEN("all elements are accessible")
            {
                for (auto i = 0; i < kMapSize; ++i)
                {
                    auto val = map.getByIndex(i);
                    REQUIRE(val != nullptr);
                    REQUIRE(*val == KeyToValue(kMinKey + i));
                    ++count;
                }
                REQUIRE(count == kMapSize);
            }
        }

        WHEN("map is moved")
        {
            Map movedMap = std::move(map);

            THEN("the moved map is not empty")
            {
                REQUIRE(movedMap.isEmpty() == false);
                REQUIRE(movedMap.size() == kMapSize);
            }

            THEN("original map is empty")
            {
                REQUIRE(map.isEmpty() == true);
                REQUIRE(map.size() == 0);
            }

            THEN("moved elements are accessible")
            {
                for (int key = kMinKey; key <= kMaxKey; ++key)
                {
                    auto value = movedMap.get(key);
                    REQUIRE(value != nullptr);
                    REQUIRE(*value == KeyToValue(key));
                }
            }
        }
    }
}