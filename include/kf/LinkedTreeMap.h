#pragma once
#include "GenericTableAvl.h"
#include <functional>
#include <utility>
#include "DoubleLinkedList.h"

namespace kf
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // LinkedTreeMap - map container for NT kernel with predictable iteration order,
    // inspired by https://docs.oracle.com/javase/8/docs/api/java/util/LinkedHashMap.html

    template<class K, class V, POOL_TYPE poolType, class LessComparer=std::less<K>>
    class LinkedTreeMap
    {
    public:
        LinkedTreeMap() noexcept = default;
        LinkedTreeMap(_Inout_ LinkedTreeMap&& another) noexcept = default;
        LinkedTreeMap& operator=(_Inout_ LinkedTreeMap&& another) noexcept = default;

        LinkedTreeMap(const LinkedTreeMap&) = delete;
        LinkedTreeMap& operator=(const LinkedTreeMap&) = delete;

        ~LinkedTreeMap()
        {
            clear();
        }

        NTSTATUS put(const K& key, const V& value)
        {
            V tmp = value;
            return put(key, std::move(tmp));
        }

        NTSTATUS put(const K& key, V&& value)
        {
            Node node(key, std::move(value));

            Node* prevNode = m_table.lookupElement(node);
            if (prevNode)
            {
                m_links.remove(*prevNode);
            }

            NTSTATUS status = m_table.insertElement(std::move(node));
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            Node* newNode = m_table.lookupElement(Node::fromKey(key));
            ASSERT(newNode);

            m_links.addLast(*newNode);

            return status;
        }

        V* get(const K& key)
        {
            Node* node = m_table.lookupElement(Node::fromKey(key));
            if (node)
            {
                return &node->m_value;
            }

            return nullptr;
        }

        const V* get(const K& key) const
        {
            return const_cast<LinkedTreeMap*>(this)->get(key);
        }

        V* getByIndex(const ULONG index)
        {
            ULONG currentIndex = 0;

            for (auto it = m_links.iterator(); it.hasNext();)
            {
                auto node = it.next();

                if (currentIndex == index)
                {
                    return &node->m_value;
                }

                ++currentIndex;
            }

            return nullptr;
        }

        const V* getByIndex(const ULONG index) const 
        {
            return const_cast<LinkedTreeMap*>(this)->getByIndex(index);
        }

        void clear()
        {
            m_links.clear();
            m_table.clear();
        }

        bool containsKey(const K& key) const
        {
            return m_table.lookupElement(Node::fromKey(key)) != nullptr;
        }

        int size() const
        {
            return m_table.number();
        }

        bool isEmpty() const
        {
            return m_table.isEmpty();
        }

        bool remove(const K& key)
        {
            auto node = m_table.lookupElement(Node::fromKey(key));
            if (node == nullptr)
            {
                return false;
            }

            m_links.remove(*node);
            return m_table.deleteElement(*node);
        }

    private:
        struct Node
        {
            static const Node& fromKey(const K& key)
            {
                return reinterpret_cast<const Node&>(key);
            }

            Node(const K& key, V&& value) : m_key(key), m_value(std::move(value))
            {
            }

            Node(Node&& another) noexcept = default;

            bool operator<(const Node& another) const
            {
                LessComparer lessComparer;
                return lessComparer(m_key, another.m_key);
            }

            K m_key;
            V m_value;
            DoubleLinkedListEntry m_listEntry;
        };

    private:
        GenericTableAvl<Node, poolType> m_table;
        DoubleLinkedList<Node, &Node::m_listEntry> m_links;
    };
}
