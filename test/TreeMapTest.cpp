#include "pch.h"
#include <kf/TreeMap.h>

using namespace kf;

namespace
{
    using IntTreeMap = kf::TreeMap<int, int, PagedPool>;

    constexpr int keyToValue(int key)
    {
        return key * 10;
    }

    constexpr std::array kKeys = { 1, 5, 3, 4, 2 };
    constexpr std::array kNonExistingKeys = { -1, 0, 6, 7, 8 };
}

SCENARIO("TreeMap: all methods")
{
    IntTreeMap map;

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
            auto contains = map.containsKey(kKeys.front());

            THEN("the value is false")
            {
                REQUIRE(!contains);
            }
        }

        WHEN("get is called")
        {
            auto value = map.get(kKeys.front());

            THEN("the value is nullptr")
            {
                REQUIRE(!value);
            }
        }

        WHEN("remove is called")
        {
            auto removed = map.remove(kKeys.front());

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
        for (int key : kKeys)
        {
            REQUIRE_NT_SUCCESS(map.put(key, keyToValue(key)));
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
                REQUIRE(map.size() == kKeys.size());
            }
        }

        WHEN("containsKey is called for existing key")
        {
            THEN("all inserted keys are found")
            {
                for (int key : kKeys)
                {
                    REQUIRE(map.containsKey(key));
                }
            }
        }

        WHEN("containsKey is called for non-existing key")
        {
            THEN("no key is found")
            {
                for (int key : kNonExistingKeys)
                {
                    REQUIRE(!map.containsKey(key));
                }
            }
        }

        WHEN("get is called for existing key")
        {
            THEN("the value should be valid")
            {
                for (int key : kKeys)
                {
                    auto value = map.get(key);
                    REQUIRE(value);
                    REQUIRE(*value == keyToValue(key));
                }
            }
        }

        WHEN("get is called for non-existing key")
        {
            THEN("the value should be nullptr")
            {
                for (int key : kNonExistingKeys)
                {
                    auto value = map.get(key);
                    REQUIRE(!value);
                }
            }
        }

        WHEN("getByIndex is called for existing index")
        {
            auto value = map.getByIndex(0);

            THEN("the value should be valid")
            {
                REQUIRE(value);
                REQUIRE(*value == keyToValue(std::ranges::min(kKeys)));
            }
        }

        WHEN("getByIndex is called for non-existing index")
        {
            auto value = map.getByIndex(static_cast<ULONG>(kKeys.size()));

            THEN("the value should be nullptr")
            {
                REQUIRE(!value);
            }
        }

        WHEN("put is called for new key")
        {
            int newKey = kNonExistingKeys.front();
            int newValue = keyToValue(newKey);
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
                REQUIRE(map.size() == kKeys.size() + 1);
            }
        }

        WHEN("put is called for existing key")
        {
            int newValue = keyToValue(kKeys.front()) + 100;
            auto status = map.put(kKeys.front(), newValue);

            THEN("the put should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("the value should be updated")
            {
                auto value = map.get(kKeys.front());
                REQUIRE(value);
                REQUIRE(*value == newValue);
            }
        }

        WHEN("remove is called for existing key")
        {
            auto removed = map.remove(kKeys.front());

            THEN("the removal should succeed")
            {
                REQUIRE(removed);
            }
            
            THEN("the key is no longer in the map")
            {
                auto value = map.get(kKeys.front());
                REQUIRE(!value);
            }

            THEN("the size is reduced")
            {
                REQUIRE(map.size() == kKeys.size() - 1);
            }
        }

        WHEN("remove is called for non-existing key")
        {
            auto removed = map.remove(kNonExistingKeys.front());

            THEN("the removal should fail")
            {
                REQUIRE(!removed);
            }

            THEN("the size remains unchanged")
            {
                REQUIRE(map.size() == kKeys.size());
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
                for (int key : kKeys)
                {
                    REQUIRE(!map.get(key));
                }
            }
        }

        WHEN("iterate over map using getByIndex")
        {
            THEN("all elements are accessible and sorted in ascending order")
            {
                std::array<int, kKeys.size()> sortedKeys = kKeys;
                std::ranges::sort(sortedKeys);

                for (auto i = 0; i < kKeys.size(); ++i)
                {
                    auto value = map.getByIndex(i);
                    REQUIRE(value);
                    REQUIRE(*value == keyToValue(sortedKeys[i]));
                }
            }
        }

        WHEN("map is moved by constructor")
        {
            IntTreeMap destinationMap = std::move(map);

            THEN("destinationMap map is not empty")
            {
                REQUIRE(!destinationMap.isEmpty());
                REQUIRE(destinationMap.size() == kKeys.size());
            }

            THEN("original map is empty")
            {
                REQUIRE(map.isEmpty());
                REQUIRE(map.size() == 0);
            }

            THEN("moved elements are accessible")
            {
                for (int key : kKeys)
                {
                    auto value = destinationMap.get(key);
                    REQUIRE(value);
                    REQUIRE(*value == keyToValue(key));
                }
            }
        }

        WHEN("map is moved by operator=")
        {
            IntTreeMap destinationMap;
            destinationMap = std::move(map);

            THEN("destinationMap map is not empty")
            {
                REQUIRE(!destinationMap.isEmpty());
                REQUIRE(destinationMap.size() == kKeys.size());
            }

            THEN("original map is empty")
            {
                REQUIRE(map.isEmpty());
                REQUIRE(map.size() == 0);
            }

            THEN("moved elements are accessible")
            {
                for (int key : kKeys)
                {
                    auto value = destinationMap.get(key);
                    REQUIRE(value);
                    REQUIRE(*value == keyToValue(key));
                }
            }
        }
    }
}

namespace
{
    // Counts the number of times an object is constructed and destructed.
    struct LifecycleCounter
    {
    public:
        LifecycleCounter()
        {
            ++s_constructCount;
        }

        LifecycleCounter(LifecycleCounter&&) noexcept
        {
            ++s_constructCount;
        }

        LifecycleCounter& operator=(LifecycleCounter&& other) noexcept = default;

        ~LifecycleCounter()
        {
            ++s_destructCount;
        }

        LifecycleCounter(const LifecycleCounter&) = delete;
        LifecycleCounter& operator=(const LifecycleCounter&) = delete;

        static void resetCounters()
        {
            s_constructCount = 0;
            s_destructCount = 0;
        }

        static bool areAllObjectsDestructed()
        {
            return s_constructCount == s_destructCount;
        }

    private:
        static inline int s_constructCount = 0;
        static inline int s_destructCount = 0;
    };

    using LifecycleCounterTreeMap = kf::TreeMap<int, LifecycleCounter, PagedPool>;
}

SCENARIO("TreeMap: lifecycle check")
{
    LifecycleCounter::resetCounters();
    
    GIVEN("map with one item")
    {
        LifecycleCounterTreeMap map;

        REQUIRE_NT_SUCCESS(map.put(kKeys.front(), LifecycleCounter{}));
        REQUIRE(!LifecycleCounter::areAllObjectsDestructed());

        WHEN("item is removed by key")
        {
            auto removed = map.remove(kKeys.front());

            THEN("all objects are destructed")
            {
                REQUIRE(removed);
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }

        WHEN("item is removed by object")
        {
            auto value = map.get(kKeys.front());
            auto removed = map.removeByObject(value);

            THEN("all objects are destructed")
            {
                REQUIRE(removed);
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }

        WHEN("map is cleared")
        {
            map.clear();

            THEN("all objects are destructed")
            {
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }

    GIVEN("map with multiple items")
    {
        {
            LifecycleCounterTreeMap map;

            for (int key : kKeys)
            {
                map.put(key, LifecycleCounter{});
            }

            REQUIRE(!LifecycleCounter::areAllObjectsDestructed());
        }

        WHEN("map has gone out of scope")
        {
            THEN("all objects are destructed")
            {
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }

    GIVEN("map value overwrite")
    {
        LifecycleCounterTreeMap map;

        REQUIRE_NT_SUCCESS(map.put(kKeys.front(), LifecycleCounter{}));
        REQUIRE(!LifecycleCounter::areAllObjectsDestructed());

        WHEN("value is overwritten for the same key")
        {
            REQUIRE_NT_SUCCESS(map.put(kKeys.front(), LifecycleCounter{}));

            THEN("all objects are not destructed yet")
            {
                REQUIRE(!LifecycleCounter::areAllObjectsDestructed());
            }

            THEN("removing the key destructs remaining objects")
            {
                auto removed = map.remove(kKeys.front());
                REQUIRE(removed);
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }

    GIVEN("map with multiple items moved to another map")
    {
        LifecycleCounterTreeMap destinationMap;

        {
            LifecycleCounterTreeMap map;

            for (int key : kKeys)
            {
                map.put(key, LifecycleCounter{});
            }

            destinationMap = std::move(map);

            REQUIRE(!LifecycleCounter::areAllObjectsDestructed());
        }

        WHEN("original map has gone out of scope but destination map lives")
        {
            THEN("all objects are not destructed")
            {
                REQUIRE(!LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }

    GIVEN("map with multiple items moved to another map")
    {
        LifecycleCounterTreeMap map;

        for (int key : kKeys)
        {
            map.put(key, LifecycleCounter{});
        }

        REQUIRE(!LifecycleCounter::areAllObjectsDestructed());

        {
            LifecycleCounterTreeMap destinationMap = std::move(map);
        }

        WHEN("destination map has gone out of scope but original map lives")
        {
            THEN("all objects are destructed")
            {
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }
}