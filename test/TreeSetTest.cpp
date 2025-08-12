#include "pch.h"
#include <kf/TreeSet.h>

using namespace kf;
using Set = kf::TreeSet<int, PagedPool>;

namespace
{
    constexpr int KeyToValue(int key)
    {
        return key * 10;
    }

    constexpr int kMinElem = 1;
    constexpr int kMaxElem = 5;
    constexpr int kSetSize = kMaxElem - kMinElem + 1;
}

SCENARIO("TreeSet: all methods")
{
    Set set;

    GIVEN("empty set")
    {
        WHEN("isEmpty is called")
        {
            auto isEmpty = set.isEmpty();

            THEN("the value is true")
            {
                REQUIRE(isEmpty == true);
            }
        }

        WHEN("size is called")
        {
            auto size = set.size();

            THEN("the size is 0")
            {
                REQUIRE(size == 0);
            }
        }

        WHEN("contains is called")
        {
            auto contains = set.contains(kMinElem);

            THEN("the value is false")
            {
                REQUIRE(contains == false);
            }
        }

        WHEN("find is called")
        {
            auto found = set.find(kMinElem);

            THEN("the value is nullptr")
            {
                REQUIRE(found == nullptr);
            }
        }

        WHEN("remove is called")
        {
            auto removed = set.remove(kMinElem);

            THEN("the removal should fail")
            {
                REQUIRE(removed == false);
            }
        }

        WHEN("clear is called")
        {
            set.clear();

            THEN("set is still empty")
            {
                REQUIRE(set.isEmpty() == true);
                REQUIRE(set.size() == 0);
            }
        }
    }

    GIVEN("set with elements")
    {
        for (int elem = kMinElem; elem <= kMaxElem; ++elem)
        {
            REQUIRE_NT_SUCCESS(set.add(elem));
        }

        WHEN("isEmpty is called")
        {
            auto isEmpty = set.isEmpty();

            THEN("the value is false")
            {
                REQUIRE(isEmpty == false);
            }
        }

        WHEN("size is called")
        {
            auto size = set.size();

            THEN("the size is valid")
            {
                REQUIRE(size == kSetSize);
            }
        }

        WHEN("contains is called for existing element")
        {
            auto contains = set.contains(kMinElem + 1);

            THEN("the value should be true")
            {
                REQUIRE(contains == true);
            }
        }

        WHEN("contains is called for a non-existing element")
        {
            auto contains = set.contains(kMaxElem + 1);

            THEN("the value should be false")
            {
                REQUIRE(contains == false);
            }
        }

        WHEN("find is called for existing element")
        {
            auto found = set.find(kMinElem + 1);

            THEN("the value should be valid")
            {
                REQUIRE(found != nullptr);
                REQUIRE(*found == kMinElem + 1);
            }
        }

        WHEN("find is called for a non-existing element")
        {
            auto found = set.find(kMaxElem + 1);

            THEN("the value should be nullptr")
            {
                REQUIRE(found == nullptr);
            }
        }

        WHEN("remove is called for existing element")
        {
            auto removed = set.remove(kMinElem);

            THEN("the removal should succeed")
            {
                REQUIRE(removed == true);
            }

            THEN("the element is no longer in the set")
            {
                auto contains = set.contains(kMinElem);
                REQUIRE(contains == false);
            }

            THEN("the size is reduced")
            {
                REQUIRE(set.size() == kSetSize - 1);
            }
        }

        WHEN("remove is called for a non-existing element")
        {
            auto removed = set.remove(kMaxElem + 1);

            THEN("the removal should fail")
            {
                REQUIRE(removed == false);
            }
        }

        WHEN("add is called for new element")
        {
            int newElem = kMaxElem + 1;
            auto status = set.add(newElem);

            THEN("the add should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("the element should be accessible")
            {
                auto contains = set.contains(newElem);
                REQUIRE(contains == true);
            }

            THEN("the size should increase")
            {
                REQUIRE(set.size() == kSetSize + 1);
            }
        }

        WHEN("add is called for existing element")
        {
            auto status = set.add(kMinElem);

            THEN("the add should succeed (idempotent)")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("the element should still be accessible")
            {
                auto contains = set.contains(kMinElem);
                REQUIRE(contains == true);
            }
        }

        WHEN("clear is called")
        {
            set.clear();

            THEN("set is empty")
            {
                REQUIRE(set.isEmpty() == true);
                REQUIRE(set.size() == 0);
            }

            THEN("valid element is no longer accessible")
            {
                REQUIRE(set.contains(kMinElem) == false);
            }
        }

        WHEN("set is moved")
        {
            Set movedSet = std::move(set);

            THEN("the moved set is not empty")
            {
                REQUIRE(movedSet.isEmpty() == false);
                REQUIRE(movedSet.size() == kSetSize);
            }

            THEN("original set is empty")
            {
                REQUIRE(set.isEmpty() == true);
                REQUIRE(set.size() == 0);
            }

            THEN("moved elements are accessible")
            {
                for (int elem = kMinElem; elem <= kMaxElem; ++elem)
                {
                    auto contains = movedSet.contains(elem);
                    REQUIRE(contains == true);
                }
            }
        }
    }
}

SCENARIO("TreeSetIterator")
{
    Set set;

    GIVEN("empty set")
    {
        WHEN("TreeSetIterator is created")
        {
            auto it = set.iterator();

            THEN("hasNext returns false")
            {
                REQUIRE(it.hasNext() == false);
            }

            THEN("next returns a reference to nullptr")
            {
                auto& next = it.next();
                REQUIRE(&next == nullptr);
            }
        }
    }

    GIVEN("set with elements")
    {
        for (int elem = kMinElem; elem <= kMaxElem; ++elem)
        {
            REQUIRE_NT_SUCCESS(set.add(elem));
        }

        WHEN("TreeSetIterator is created")
        {
            auto it = set.iterator();

            int expectedValue = kMinElem;
            int count = 0;

            THEN("it should enumerate all elements in order")
            {
                while (it.hasNext())
                {
                    int value = it.next();
                    REQUIRE(value == expectedValue);
                    ++expectedValue;
                    ++count;
                }
                REQUIRE(count == (kMaxElem - kMinElem + 1));
            }
        }
    }
}