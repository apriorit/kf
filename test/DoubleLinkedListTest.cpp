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
