#include "pch.h"
#include <kf/TreeMap.h>

using namespace kf;

namespace
{
    using Map = kf::TreeMap<int, int, PagedPool>;

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

    // Counts the number of times an object is destructed.
    // Doesn't count the destructor calls for moved objects.
    struct DestructionCounter
    {
        static inline int sDestructCount = 0;

        int key = 0;

        explicit DestructionCounter(int k)
            : key(k)
        {
            ASSERT(key != 0);
        }

        ~DestructionCounter()
        {
            if (key != 0)
            {
                ++sDestructCount;
            }
        }

        DestructionCounter(DestructionCounter&& other) noexcept
            : key(other.key)
        {
            other.key = 0;
        }

        DestructionCounter& operator=(DestructionCounter&& other) noexcept
        {
            if (this != &other)
            {
                key = other.key;
                other.key = 0;
            }
            return *this;
        }

        DestructionCounter(const DestructionCounter&) = delete;
        DestructionCounter& operator=(const DestructionCounter&) = delete;
    };

    using DestructionMap = kf::TreeMap<int, DestructionCounter, PagedPool>;
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
                REQUIRE(isEmpty);
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
                REQUIRE(!contains);
            }
        }

        WHEN("get is called")
        {
            auto value = map.get(kMinKey);

            THEN("the value is nullptr")
            {
                REQUIRE(!value);
            }
        }

        WHEN("remove is called")
        {
            auto removed = map.remove(kMinKey);

            THEN("the removal should fail")
            {
                REQUIRE(!removed);
            }
        }

        WHEN("clear is called")
        {
            map.clear();

            THEN("map is still empty")
            {
                REQUIRE(map.isEmpty());
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
                REQUIRE(!map.isEmpty());
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
                REQUIRE(contains);
            }
        }

        WHEN("containsKey is called for non-existing key")
        {
            auto contains = map.containsKey(kMinKey - 1);

            THEN("the value should be false")
            {
                REQUIRE(!contains);
            }
        }

        WHEN("get is called for existing key")
        {
            auto value = map.get(kMaxKey / 2);

            THEN("the value should be valid")
            {
                REQUIRE(value);
                REQUIRE(*value == KeyToValue(kMaxKey / 2));
            }
        }

        WHEN("get is called for non-existing key")
        {
            auto value = map.get(kMaxKey + 2);

            THEN("the value should be nullptr")
            {
                REQUIRE(!value);
            }
        }

        WHEN("getByIndex is called for existing index")
        {
            auto value = map.getByIndex(0);

            THEN("the value should be valid")
            {
                REQUIRE(value);
                REQUIRE(*value == KeyToValue(kMinKey));
            }
        }

        WHEN("getByIndex is called for non-existing index")
        {
            auto value = map.getByIndex(kMapSize + 1);

            THEN("the value should be nullptr")
            {
                REQUIRE(!value);
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
                REQUIRE(value);
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
                REQUIRE(value);
                REQUIRE(*value == newValue);
            }
        }

        WHEN("remove is called for existing key")
        {
            auto removed = map.remove(kMinKey);

            THEN("the removal should succeed")
            {
                REQUIRE(removed);
            }
            
            THEN("the key is no longer in the map")
            {
                auto value = map.get(kMinKey);
                REQUIRE(!value);
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
                REQUIRE(!removed);
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
                REQUIRE(map.isEmpty());
                REQUIRE(map.size() == 0);
            }
            
            THEN("valid key is no longer accessible")
            {
                REQUIRE(!map.get(kMinKey));
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
                    REQUIRE(val);
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
                REQUIRE(!movedMap.isEmpty());
                REQUIRE(movedMap.size() == kMapSize);
            }

            THEN("original map is empty")
            {
                REQUIRE(map.isEmpty());
                REQUIRE(map.size() == 0);
            }

            THEN("moved elements are accessible")
            {
                for (int key = kMinKey; key <= kMaxKey; ++key)
                {
                    auto value = movedMap.get(key);
                    REQUIRE(value);
                    REQUIRE(*value == KeyToValue(key));
                }
            }
        }
    }
}

SCENARIO("TreeMap: destruction check")
{
    constexpr int kKey1 = 1;
    constexpr int kKey2 = 2;
    constexpr int kKey3 = 3;

    GIVEN("map with one item")
    {
        DestructionCounter::sDestructCount = 0;
        DestructionMap map;

        REQUIRE_NT_SUCCESS(map.put(kKey1, DestructionCounter(kKey1)));

        WHEN("item is removed by key")
        {
            auto removed = map.remove(kKey1);

            THEN("destructor is called once")
            {
                REQUIRE(removed);
                REQUIRE(DestructionCounter::sDestructCount == 1);
            }
        }
    }

    GIVEN("map with one item")
    {
        DestructionCounter::sDestructCount = 0;
        DestructionMap map;

        REQUIRE_NT_SUCCESS(map.put(kKey2, DestructionCounter(kKey2)));
        auto value = map.get(kKey2);

        WHEN("item is removed by value")
        {
            auto removed = map.removeByObject(value);

            THEN("destructor is called once")
            {
                REQUIRE(removed);
                REQUIRE(DestructionCounter::sDestructCount == 1);
            }
        }
    }

    GIVEN("map with multiple items")
    {
        DestructionCounter::sDestructCount = 0;

        WHEN("map is going out of scope")
        {
            DestructionMap map;
            map.put(kKey1, DestructionCounter(kKey1));
            map.put(kKey2, DestructionCounter(kKey2));
            map.put(kKey3, DestructionCounter(kKey3));
        }

        THEN("destructors are called for all items")
        {
            REQUIRE(DestructionCounter::sDestructCount == 3);
        }
    }

    GIVEN("map with multiple items")
    {
        DestructionCounter::sDestructCount = 0;

        DestructionMap movedMap;
        {
            DestructionMap map;
            map.put(kKey1, std::move(DestructionCounter(kKey1)));
            map.put(kKey2, std::move(DestructionCounter(kKey2)));

            WHEN("map is moved")
            {
                movedMap = std::move(map);

                THEN("destructors are not called yet")
                {
                    REQUIRE(DestructionCounter::sDestructCount == 0);
                }
            }
        }
        
        WHEN("original map is out of scope")
        {
            THEN("destructors for moved objects are not called")
            {
                REQUIRE(DestructionCounter::sDestructCount == 0);
            }
        }

        WHEN("movedMap is cleared")
        {
            movedMap.clear();

            THEN("destructors are called for moved objects")
            {
                REQUIRE(DestructionCounter::sDestructCount == 2);
            }
        }
    }
}