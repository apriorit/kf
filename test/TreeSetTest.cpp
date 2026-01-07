#include "pch.h"
#include <kf/TreeSet.h>

using namespace kf;

namespace
{
    using IntTreeSet = kf::TreeSet<int, PagedPool>;

    constexpr std::array kElements = { 1, 5, 3, 4, 2 };
    constexpr std::array kNonExistingElements = { -1, 0, 6, 7, 8 };
}

SCENARIO("TreeSet: all methods")
{
    IntTreeSet set;

    GIVEN("empty set")
    {
        WHEN("isEmpty is called")
        {
            auto isEmpty = set.isEmpty();

            THEN("the value is true")
            {
                REQUIRE(isEmpty);
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
            auto contains = set.contains(kElements.front());

            THEN("the value is false")
            {
                REQUIRE(!contains);
            }
        }

        WHEN("find is called")
        {
            auto found = set.find(kElements.front());

            THEN("the value is nullptr")
            {
                REQUIRE(found == nullptr);
            }
        }

        WHEN("remove is called")
        {
            auto removed = set.remove(kElements.front());

            THEN("the removal should fail")
            {
                REQUIRE(!removed);
            }
        }

        WHEN("clear is called")
        {
            set.clear();

            THEN("set is still empty")
            {
                REQUIRE(set.isEmpty());
                REQUIRE(set.size() == 0);
            }
        }
    }

    GIVEN("set with elements")
    {
        for (int elem : kElements)
        {
            REQUIRE_NT_SUCCESS(set.add(elem));
        }

        WHEN("isEmpty is called")
        {
            THEN("the value is false")
            {
                REQUIRE(!set.isEmpty());
            }
        }

        WHEN("size is called")
        {
            THEN("the size is valid")
            {
                REQUIRE(set.size() == kElements.size());
            }
        }

        WHEN("contains is called for existing elements")
        {
            THEN("the values should be true")
            {
                for (int elem : kElements)
                {
                    REQUIRE(set.contains(elem));
                }
            }
        }

        WHEN("contains is called for non-existing elements")
        {
            THEN("the values should be false")
            {
                for (int elem : kNonExistingElements)
                {
                    REQUIRE(!set.contains(elem));
                }
            }
        }

        WHEN("find is called for existing element")
        {
            THEN("the value should be valid")
            {
                for (int elem : kElements)
                {
                    auto found = set.find(elem);
                    REQUIRE(found != nullptr);
                    REQUIRE(*found == elem);
                }
            }
        }

        WHEN("find is called for non-existing element")
        {
            THEN("the value should be nullptr")
            {
                for (int elem : kNonExistingElements)
                {
                    auto found = set.find(elem);
                    REQUIRE(found == nullptr);
                }
            }
        }

        WHEN("remove is called for existing element")
        {
            auto removed = set.remove(kElements.front());

            THEN("the removal should succeed")
            {
                REQUIRE(removed);
            }

            THEN("the element is no longer in the set")
            {
                REQUIRE(!set.contains(kElements.front()));
            }

            THEN("the size is reduced")
            {
                REQUIRE(set.size() == kElements.size() - 1);
            }
        }

        WHEN("remove is called for a non-existing element")
        {
            auto removed = set.remove(kNonExistingElements.front());

            THEN("the removal should fail")
            {
                REQUIRE(!removed);
            }
        }

        WHEN("add is called for new element")
        {
            int newElem = kNonExistingElements.front();
            auto status = set.add(newElem);

            THEN("the add should succeed")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("the element should be accessible")
            {
                REQUIRE(set.contains(newElem));
            }

            THEN("the size should increase")
            {
                REQUIRE(set.size() == kElements.size() + 1);
            }
        }

        WHEN("add is called for existing element")
        {
            auto status = set.add(kElements.front());

            THEN("the add should succeed (idempotent)")
            {
                REQUIRE_NT_SUCCESS(status);
            }

            THEN("the element should still be accessible")
            {
                REQUIRE(set.contains(kElements.front()));
            }
        }

        WHEN("clear is called")
        {
            set.clear();

            THEN("set is empty")
            {
                REQUIRE(set.isEmpty());
                REQUIRE(set.size() == 0);
            }

            THEN("valid element is no longer accessible")
            {
                for (int elem : kElements)
                {
                    REQUIRE(!set.contains(elem));
                }
            }
        }

        WHEN("iterate over set using iterator")
        {
            THEN("all elements are accessible and sorted in ascending order")
            {
                std::array<int, kElements.size()> sorted = kElements;
                std::ranges::sort(sorted);

                auto it = set.iterator();
                size_t idx = 0;
                while (it.hasNext())
                {
                    int value = it.next();
                    REQUIRE(value == sorted[idx]);
                    ++idx;
                }
                REQUIRE(idx == sorted.size());
            }
        }

        WHEN("set is moved by constructor")
        {
            IntTreeSet destinationSet = std::move(set);

            THEN("destination set is not empty")
            {
                REQUIRE(!destinationSet.isEmpty());
                REQUIRE(destinationSet.size() == kElements.size());
            }

            THEN("original set is empty")
            {
                REQUIRE(set.isEmpty());
                REQUIRE(set.size() == 0);
            }

            THEN("moved elements are accessible")
            {
                for (int elem : kElements)
                {
                    REQUIRE(destinationSet.contains(elem));
                }
            }
        }

        WHEN("set is moved by operator=")
        {
            IntTreeSet destinationSet;
            destinationSet = std::move(set);

            THEN("destination set is not empty")
            {
                REQUIRE(!destinationSet.isEmpty());
                REQUIRE(destinationSet.size() == kElements.size());
            }

            THEN("original set is empty")
            {
                REQUIRE(set.isEmpty());
                REQUIRE(set.size() == 0);
            }

            THEN("moved elements are accessible")
            {
                for (int elem : kElements)
                {
                    REQUIRE(destinationSet.contains(elem));
                }
            }
        }
    }
}

namespace
{
    struct LifecycleCounter
    {
    public:
        LifecycleCounter()
            : m_id(++s_nextId)
        {
            ++s_constructCount;
        }

        LifecycleCounter(LifecycleCounter&& other) noexcept
            : m_id(other.m_id)
        {
            ++s_constructCount;
        }

        explicit LifecycleCounter(int id)
            : m_id(id)
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

        friend bool operator<(const LifecycleCounter& lhs, const LifecycleCounter& rhs)
        {
            return lhs.m_id < rhs.m_id;
        }

    private:
        int m_id = 0;
        static inline int s_constructCount = 0;
        static inline int s_destructCount = 0;
        static inline int s_nextId = 0;
    };

    using LifecycleCounterTreeSet = kf::TreeSet<LifecycleCounter, PagedPool>;
}

SCENARIO("TreeSet: lifecycle check")
{
    LifecycleCounter::resetCounters();

    GIVEN("set with one item")
    {
        LifecycleCounterTreeSet set;
        const int id = 123;

        REQUIRE_NT_SUCCESS(set.add(LifecycleCounter{id}));
        REQUIRE(!LifecycleCounter::areAllObjectsDestructed());

        WHEN("item is removed by value")
        {
            auto removed = set.remove(LifecycleCounter{id});

            THEN("all objects are destructed")
            {
                REQUIRE(removed);
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }

        WHEN("set is cleared")
        {
            set.clear();

            THEN("all objects are destructed")
            {
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }

    GIVEN("set with multiple items")
    {
        {
            LifecycleCounterTreeSet set;

            REQUIRE_NT_SUCCESS(set.add(LifecycleCounter{}));
            REQUIRE_NT_SUCCESS(set.add(LifecycleCounter{}));
            REQUIRE_NT_SUCCESS(set.add(LifecycleCounter{}));

            REQUIRE(!LifecycleCounter::areAllObjectsDestructed());
        }

        WHEN("set has gone out of scope")
        {
            THEN("all objects are destructed")
            {
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }

    GIVEN("set with multiple items moved to another set")
    {
        LifecycleCounterTreeSet destinationSet;

        {
            LifecycleCounterTreeSet set;

            REQUIRE_NT_SUCCESS(set.add(LifecycleCounter{}));
            REQUIRE_NT_SUCCESS(set.add(LifecycleCounter{}));
            REQUIRE_NT_SUCCESS(set.add(LifecycleCounter{}));

            destinationSet = std::move(set);

            REQUIRE(!LifecycleCounter::areAllObjectsDestructed());
        }

        WHEN("original set has gone out of scope but destination set lives")
        {
            THEN("all objects are not destructed")
            {
                REQUIRE(!LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }

    GIVEN("set with multiple items moved to another set (destination goes out of scope)")
    {
        LifecycleCounterTreeSet localSet;

        REQUIRE_NT_SUCCESS(localSet.add(LifecycleCounter{}));
        REQUIRE_NT_SUCCESS(localSet.add(LifecycleCounter{}));
        REQUIRE_NT_SUCCESS(localSet.add(LifecycleCounter{}));

        REQUIRE(!LifecycleCounter::areAllObjectsDestructed());

        {
            LifecycleCounterTreeSet destinationSet = std::move(localSet);
        }

        WHEN("destination set has gone out of scope but original set lives")
        {
            THEN("all objects are destructed")
            {
                REQUIRE(LifecycleCounter::areAllObjectsDestructed());
            }
        }
    }
}
