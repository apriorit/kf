#include "pch.h"
#include <kf/DoubleLinkedList.h>

using namespace kf;

struct TestNode
{
    int value = 0;
    DoubleLinkedListEntry entry;

    explicit TestNode(int v)
        : value(v)
    { }
};

using List = DoubleLinkedList<TestNode, &TestNode::entry>;
using Iterator = DoubleLinkedListIterator<TestNode, &TestNode::entry>;
using ConstIterator = DoubleLinkedListConstIterator<TestNode, &TestNode::entry>;

static_assert(std::bidirectional_iterator<List::iterator_type>);
static_assert(std::bidirectional_iterator<List::const_iterator>);
static_assert(std::ranges::bidirectional_range<List>);
static_assert(std::ranges::bidirectional_range<const List>);
static_assert(std::convertible_to<List::iterator_type, List::const_iterator>);
static_assert(!std::convertible_to<List::const_iterator, List::iterator_type>);
static_assert(std::same_as<std::ranges::range_reference_t<List>, TestNode&>);
static_assert(std::same_as<std::ranges::range_reference_t<const List>, const TestNode&>);

SCENARIO("DoubleLinkedList standard iterators")
{
    GIVEN("an empty list")
    {
        List list;
        const List& constList = list;

        WHEN("its ranges are inspected")
        {
            THEN("forward and reverse ranges are empty")
            {
                REQUIRE(list.begin() == list.end());
                REQUIRE(constList.begin() == constList.end());
                REQUIRE(list.rbegin() == list.rend());
                REQUIRE(constList.crbegin() == constList.crend());
                REQUIRE(std::ranges::distance(list) == 0);
            }
        }
    }

    GIVEN("a list with one element")
    {
        TestNode node1{ 1 };
        TestNode node2{ 2 };
        TestNode node3{ 3 };
        List list;
        list.addLast(node1);

        WHEN("its range is inspected")
        {
            THEN("it contains exactly that element")
            {
                REQUIRE(&*list.begin() == &node1);
                REQUIRE(&*--list.end() == &node1);
                REQUIRE(++list.begin() == list.end());
            }
        }

        WHEN("other elements are inserted")
        {
            auto first = list.begin();
            list.addLast(node2);
            list.addLast(node3);

            THEN("the existing iterator remains valid")
            {
                REQUIRE(&*first == &node1);
            }
        }
    }

    GIVEN("a list with three elements")
    {
        TestNode node1{ 1 };
        TestNode node2{ 2 };
        TestNode node3{ 3 };
        List list;
        list.addLast(node1);
        list.addLast(node2);
        list.addLast(node3);
        const List& constList = list;

        WHEN("an iterator is converted to a const iterator")
        {
            auto it = list.begin();
            List::const_iterator constIt = it;

            THEN("the iterators compare equal in both directions")
            {
                REQUIRE(it == constIt);
                REQUIRE(constIt == it);
            }
        }

        WHEN("an iterator is advanced and decremented")
        {
            auto it = list.begin();

            THEN("it traverses the list in both directions")
            {
                REQUIRE(&*it++ == &node1);
                REQUIRE(it->value == 2);
                REQUIRE(&*it-- == &node2);
                REQUIRE(&*it == &node1);
                it = list.end();
                REQUIRE(&*--it == &node3);
                REQUIRE(&*it-- == &node3);
                REQUIRE(&*it == &node2);
            }
        }

        WHEN("elements are modified using a range-based loop")
        {
            for (auto& node : list)
            {
                node.value *= 2;
            }

            THEN("standard algorithms and views see the modified values")
            {
                const int expected[] = { 2, 4, 6 };
                const int reverseExpected[] = { 6, 4, 2 };
                REQUIRE(std::ranges::equal(constList | std::views::transform([](const TestNode& node) { return node.value; }), expected));
                REQUIRE(std::find_if(list.begin(), list.end(), [](const TestNode& node) { return node.value == 4; }) == ++list.begin());
                REQUIRE(std::ranges::distance(list) == 3);
                REQUIRE(std::ranges::equal(constList | std::views::reverse | std::views::transform([](const TestNode& node) { return node.value; }), reverseExpected));
                REQUIRE(&*list.rbegin() == &node3);
                REQUIRE(&*constList.crbegin() == &node3);
                REQUIRE(constList.crend().base() == constList.cbegin());
            }
        }

        WHEN("the middle element is removed")
        {
            auto first = list.begin();
            auto last = --list.end();
            list.remove(node2);

            THEN("iterators to the remaining elements stay valid")
            {
                REQUIRE(&*first == &node1);
                REQUIRE(&*last == &node3);
                REQUIRE(++first == last);
            }
        }

        WHEN("the list is cleared and an element is reinserted")
        {
            list.clear();
            list.addLast(node2);

            THEN("the range contains only the reinserted element")
            {
                REQUIRE(&*list.begin() == &node2);
                REQUIRE(&*--list.end() == &node2);
                REQUIRE(++list.begin() == list.end());
            }
        }
    }
}

SCENARIO("DoubleLinkedList: all methods")
{
    TestNode node1{ 1 };
    TestNode node2{ 2 };
    TestNode node3{ 3 };
    TestNode node4{ 4 };

    List list;

    WHEN("isEmpty and size are checked")
    {
        THEN("list is empty")
        {
            REQUIRE(list.isEmpty());
            REQUIRE(list.size() == 0);
        }
    }

    WHEN("addFirst is used")
    {
        list.addFirst(node1);

        THEN("one element is at front")
        {
            REQUIRE(!list.isEmpty());
            REQUIRE(list.size() == 1);
            auto it = list.iterator();
            REQUIRE(it.hasNext());
            auto* node = it.next();
            REQUIRE(node->value == 1);
        }
    }

    WHEN("addLast is used")
    {
        list.addFirst(node1);
        list.addLast(node2);

        THEN("elements are in correct order")
        {
            REQUIRE(list.size() == 2);
            int expected[] = { 1, 2 };
            int idx = 0;
            for (auto it = list.iterator(); it.hasNext(); )
            {
                auto* node = it.next();
                REQUIRE(node->value == expected[idx++]);
            }
        }
    }

    WHEN("addAfter is used")
    {
        list.addFirst(node1);
        list.addLast(node2);
        list.addAfter(node1, node3);

        THEN("element is inserted after given node")
        {
            int expected[] = { 1, 3, 2 };
            int idx = 0;
            for (auto it = list.iterator(); it.hasNext(); )
            {
                auto* node = it.next();
                REQUIRE(node->value == expected[idx++]);
            }
        }
    }

    WHEN("addBefore is used")
    {
        list.addFirst(node1);
        list.addLast(node2);
        list.addAfter(node1, node3);
        list.addBefore(node2, node4);

        THEN("element is inserted before given node")
        {
            int expected[] = { 1, 3, 4, 2 };
            int idx = 0;
            for (auto it = list.iterator(); it.hasNext(); )
            {
                auto* node = it.next();
                REQUIRE(node->value == expected[idx++]);
            }
        }
    }

    WHEN("indexOf is used")
    {
        list.addFirst(node1);
        list.addLast(node2);
        list.addAfter(node1, node3);
        list.addBefore(node2, node4);

        THEN("indices are correct")
        {
            REQUIRE(list.indexOf(node1) == 0);
            REQUIRE(list.indexOf(node3) == 1);
            REQUIRE(list.indexOf(node4) == 2);
            REQUIRE(list.indexOf(node2) == 3);
        }
    }

    WHEN("contains is used")
    {
        list.addFirst(node1);
        list.addLast(node2);
        list.addAfter(node1, node3);
        list.addBefore(node2, node4);

        THEN("contains returns correct result")
        {
            REQUIRE(list.contains(node3));
            TestNode node5{ 5 };
            REQUIRE(!list.contains(node5));
        }
    }

    WHEN("remove is used")
    {
        list.addFirst(node1);
        list.addLast(node2);
        list.addAfter(node1, node3);
        list.addBefore(node2, node4);

        bool removed = list.remove(node3);

        THEN("element is removed")
        {
            REQUIRE(removed);
            REQUIRE(list.size() == 3);
            REQUIRE(!list.contains(node3));
            REQUIRE(!list.remove(node3));
        }
    }

    WHEN("removeFirst is used")
    {
        list.addFirst(node1);
        list.addLast(node2);
        list.addAfter(node1, node3);
        list.addBefore(node2, node4);

        TestNode* first = list.removeFirst();

        THEN("first element is removed")
        {
            REQUIRE(first == &node1);
            REQUIRE(list.size() == 3);
        }
    }

    WHEN("clear is used")
    {
        list.addFirst(node1);
        list.addLast(node2);
        list.addAfter(node1, node3);
        list.addBefore(node2, node4);

        list.clear();

        THEN("the list is empty")
        {
            REQUIRE(list.isEmpty());
            REQUIRE(list.size() == 0);
        }
    }

    WHEN("move constructor is used")
    {
        list.addLast(node1);
        list.addLast(node2);

        List list2{ std::move(list) };

        THEN("elements are moved")
        {
            REQUIRE(list2.size() == 2);
            REQUIRE(list.isEmpty());
        }
    }

    WHEN("move assignment is used")
    {
        list.addLast(node1);
        list.addLast(node2);

        List list2;
        list2.addLast(node3);
        list2 = std::move(list);

        THEN("elements are moved")
        {
            REQUIRE(list2.size() == 2);
            REQUIRE(list.isEmpty());
        }
    }
}

SCENARIO("DoubleLinkedListConstIterator")
{
    TestNode node1{ 1 };
    TestNode node2{ 2 };
    TestNode node3{ 3 };

    List list;

    WHEN("const iterator is used on an empty list")
    {
        auto it = static_cast<const List&>(list).iterator();

        THEN("hasNext is false")
        {
            REQUIRE(!it.hasNext());
        }
    }

    WHEN("const iterator is used on a non-empty list")
    {
        list.addLast(node1);
        list.addLast(node2);
        list.addLast(node3);

        auto it = static_cast<const List&>(list).iterator();

        THEN("hasNext is true and next returns elements in order")
        {
            REQUIRE(it.hasNext());
            auto* n = it.next();
            REQUIRE(n->value == 1);

            REQUIRE(it.hasNext());
            n = it.next();
            REQUIRE(n->value == 2);

            REQUIRE(it.hasNext());
            n = it.next();
            REQUIRE(n->value == 3);

            REQUIRE(!it.hasNext());
        }
    }

    WHEN("const iterator is constructed from a specific element")
    {
        list.addLast(node1);
        list.addLast(node2);
        list.addLast(node3);

        auto it = list.iterator(node2);

        THEN("next returns node3")
        {
            REQUIRE(it.hasNext());
            auto* n = it.next();
            REQUIRE(n->value == 3);

            REQUIRE(!it.hasNext());
        }
    }
}

SCENARIO("DoubleLinkedListIterator")
{
    TestNode node1{ 1 };
    TestNode node2{ 2 };
    TestNode node3{ 3 };

    List list;

    WHEN("iterator is used to modify elements")
    {
        list.addLast(node1);
        list.addLast(node2);
        list.addLast(node3);

        Iterator it = list.iterator();

        THEN("values can be changed through iterator")
        {
            int newValue = 10;
            while (it.hasNext())
            {
                auto* node = it.next();
                node->value += newValue;
                newValue += 10;
            }

            int expected[] = { 11, 22, 33 };
            int idx = 0;
            for (auto it2 = list.iterator(); it2.hasNext(); )
            {
                auto* node = it2.next();
                REQUIRE(node->value == expected[idx++]);
            }
        }
    }

    WHEN("iterator is used to remove elements")
    {
        list.addLast(node1);
        list.addLast(node2);
        list.addLast(node3);

        Iterator it = list.iterator();

        THEN("all elements can be removed using iterator")
        {
            int removeCount = 0;
            while (it.hasNext())
            {
                it.next();
                it.remove();
                ++removeCount;
            }
            REQUIRE(list.isEmpty());
            REQUIRE(list.size() == 0);
            REQUIRE(removeCount == 3);
        }
    }
}

SCENARIO("DoubleLinkedListDescendingIterator")
{
    TestNode node1{ 1 };
    TestNode node2{ 2 };
    TestNode node3{ 3 };
    TestNode node4{ 4 };

    List list;

    list.addFirst(node1);
    list.addLast(node2);
    list.addAfter(node1, node3);
    list.addBefore(node2, node4);

    WHEN("descendingIterator is used")
    {
        THEN("elements are in reverse order")
        {
            int expectedDesc[] = { 2, 4, 3, 1 };
            int idx = 0;
            for (auto it = list.descendingIterator(); it.hasNext(); )
            {
                auto* node = it.next();
                REQUIRE(node->value == expectedDesc[idx++]);
            }
        }
    }
}
