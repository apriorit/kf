#include "pch.h"
#include <kf/LinkedTreeMap.h>

using namespace kf;

namespace
{
    using LinkedIntMap = kf::LinkedTreeMap<int, int, PagedPool>;

    constexpr int keyToValue(int key)
    {
        return key * 10;
    }

    constexpr std::array<int, 5> kKeys = { 1, 5, 3, 4, 2 };
    constexpr std::array<int, 5> kNonExistingKeys = { -1, 0, 6, 7, 8 };
}

SCENARIO("LinkedTreeMap: all methods")
{
    LinkedIntMap map;

    GIVEN("empty map")
    {
        WHEN("isEmpty is called")
        {
            THEN("the value is true")
            {
                REQUIRE(map.isEmpty());
            }
        }

        WHEN("size is called")
        {
            THEN("the size is 0")
            {
                REQUIRE(map.size() == 0);
            }
        }

        WHEN("containsKey is called")
        {
            THEN("the value is false")
            {
                REQUIRE(!map.containsKey(kKeys.front()));
            }
        }

        WHEN("get is called")
        {
            THEN("the value is nullptr")
            {
                auto value = map.get(kKeys.front());
                REQUIRE(value == nullptr);
            }
        }

        WHEN("remove is called")
        {
            THEN("the removal should fail")
            {
                auto removed = map.remove(kKeys.front());
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
            THEN("values match inserted data")
            {
                for (int key : kKeys)
                {
                    auto value = map.get(key);
                    REQUIRE(value != nullptr);
                    REQUIRE(*value == keyToValue(key));
                }
            }
        }

        WHEN("get is called for non-existing key")
        {
            THEN("the value is nullptr")
            {
                for (int key : kNonExistingKeys)
                {
                    auto value = map.get(key);
                    REQUIRE(value == nullptr);
                }
            }
        }

        WHEN("getByIndex is called for valid indices")
        {
            THEN("elements follow insertion order")
            {
                for (ULONG i = 0; i < kKeys.size(); ++i)
                {
                    auto value = map.getByIndex(i);
                    REQUIRE(value != nullptr);
                    REQUIRE(*value == keyToValue(kKeys[i]));
                }
            }
        }

        WHEN("getByIndex is called for non-existing index")
        {
            auto value = map.getByIndex(static_cast<ULONG>(kKeys.size()));

            THEN("the value is nullptr")
            {
                REQUIRE(value == nullptr);
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
                REQUIRE(value != nullptr);
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
                REQUIRE(value != nullptr);
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
                REQUIRE(value == nullptr);
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
                    REQUIRE(map.get(key) == nullptr);
                }
            }
        }

        WHEN("iterate over map using getByIndex")
        {
            THEN("all elements are accessible in insertion order")
            {
                ULONG index = 0;
                for (int key : kKeys)
                {
                    auto value = map.getByIndex(index++);
                    REQUIRE(value != nullptr);
                    REQUIRE(*value == keyToValue(key));
                }
                REQUIRE(index == kKeys.size());
            }
        }

        WHEN("map is moved by constructor")
        {
            LinkedIntMap destinationMap = std::move(map);

            THEN("destination map is not empty")
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
                    REQUIRE(value != nullptr);
                    REQUIRE(*value == keyToValue(key));
                }
            }
        }

        WHEN("map is moved by operator=")
        {
            LinkedIntMap destinationMap;
            destinationMap = std::move(map);

            THEN("destination map is not empty")
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
                    REQUIRE(value != nullptr);
                    REQUIRE(*value == keyToValue(key));
                }
            }
        }
    }
}

SCENARIO("LinkedTreeMap: ordering")
{
    LinkedIntMap map;

    GIVEN("map with elements")
    {
        for (int key : kKeys)
        {
            REQUIRE_NT_SUCCESS(map.put(key, keyToValue(key)));
        }

        WHEN("iterating using getByIndex")
        {
            THEN("elements are returned in insertion order")
            {
                for (ULONG index = 0; index < kKeys.size(); ++index)
                {
                    auto value = map.getByIndex(index);
                    REQUIRE(*value == keyToValue(kKeys[index]));
                }
            }
        }

        WHEN("the middle inserted key is updated")
        {
            constexpr size_t kUpdateKeyIndex = kKeys.size() / 2;
            constexpr int kUpdateKey = kKeys[kUpdateKeyIndex];
            REQUIRE_NT_SUCCESS(map.put(kUpdateKey, keyToValue(kUpdateKey)));

            THEN("updated key is moved to the end")
            {
                for (size_t index = 0; index < kKeys.size(); ++index)
                {
                    auto value = map.getByIndex(static_cast<ULONG>(index));

                    if (index < kKeys.size() - 1)
                    {
                        if (index < kUpdateKeyIndex)
                        {
                            REQUIRE(*value == keyToValue(kKeys[index]));
                        }
                        else
                        {
                            REQUIRE(*value == keyToValue(kKeys[index + 1]));
                        }
                    }
                    else
                    {
                        REQUIRE(*value == keyToValue(kUpdateKey));
                    }
                }
            }
        }

        WHEN("the first inserted key is updated")
        {
            constexpr size_t kUpdateKeyIndex = 0;
            constexpr int kUpdateKey = kKeys[kUpdateKeyIndex];
            REQUIRE_NT_SUCCESS(map.put(kUpdateKey, keyToValue(kUpdateKey)));

            THEN("updated key is moved to the end")
            {
                for (size_t index = 0; index < kKeys.size(); ++index)
                {
                    auto value = map.getByIndex(static_cast<ULONG>(index));

                    if (index < kKeys.size() - 1)
                    {
                        REQUIRE(*value == keyToValue(kKeys[index + 1]));
                    }
                    else
                    {
                        REQUIRE(*value == keyToValue(kUpdateKey));
                    }
                }
            }
        }

        WHEN("the last inserted key is updated")
        {
            constexpr size_t kUpdateKeyIndex = kKeys.size() - 1;
            constexpr int kUpdateKey = kKeys[kUpdateKeyIndex];
            REQUIRE_NT_SUCCESS(map.put(kUpdateKey, keyToValue(kUpdateKey)));

            THEN("updated key is moved to the end")
            {
                for (size_t index = 0; index < kKeys.size(); ++index)
                {
                    auto value = map.getByIndex(static_cast<ULONG>(index));

                    if (index < kKeys.size() - 1)
                    {
                        REQUIRE(*value == keyToValue(kKeys[index]));
                    }
                    else
                    {
                        REQUIRE(*value == keyToValue(kUpdateKey));
                    }
                }
            }
        }

        WHEN("the first inserted key is removed")
        {
            constexpr size_t kRemoveKeyIndex = 0;
            constexpr int kRemoveKey = kKeys[kRemoveKeyIndex];
            REQUIRE(map.remove(kRemoveKey));

            THEN("elements preserve insertion order without removed key")
            {
                REQUIRE(map.size() == kKeys.size() - 1);

                for (size_t index = 0; index < map.size(); ++index)
                {
                    auto value = map.getByIndex(static_cast<ULONG>(index));
                    REQUIRE(*value == keyToValue(kKeys[index + 1]));
                }
            }
        }

        WHEN("the middle inserted key is removed")
        {
            constexpr size_t kRemoveKeyIndex = kKeys.size() / 2;
            constexpr int kRemoveKey = kKeys[kRemoveKeyIndex];
            REQUIRE(map.remove(kRemoveKey));

            THEN("elements preserve insertion order without removed key")
            {
                REQUIRE(map.size() == kKeys.size() - 1);

                for (size_t index = 0; index < map.size(); ++index)
                {
                    auto value = map.getByIndex(static_cast<ULONG>(index));

                    if (index < kRemoveKeyIndex)
                    {
                        REQUIRE(*value == keyToValue(kKeys[index]));
                    }
                    else
                    {
                        REQUIRE(*value == keyToValue(kKeys[index + 1]));
                    }
                }
            }
        }

        WHEN("the last inserted key is removed")
        {
            constexpr size_t kRemoveKeyIndex = kKeys.size() - 1;
            constexpr int kRemoveKey = kKeys[kRemoveKeyIndex];
            REQUIRE(map.remove(kRemoveKey));

            THEN("elements preserve insertion order without removed key")
            {
                REQUIRE(map.size() == kKeys.size() - 1);

                for (size_t index = 0; index < map.size(); ++index)
                {
                    auto value = map.getByIndex(static_cast<ULONG>(index));
                    REQUIRE(*value == keyToValue(kKeys[index]));
                }
            }
        }
    }
}
