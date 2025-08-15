#pragma once
#include "GenericTableAvl.h"
#include <functional>
#include <utility>

namespace kf
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // TreeMap - map container for NT kernel, inspired by http://docs.oracle.com/javase/7/docs/api/java/util/TreeMap.html

    template<class K, class V, POOL_TYPE poolType, class LessComparer=std::less<K>>
    class TreeMap
    {
    public:
        TreeMap()
        {
        }

        TreeMap(_Inout_ TreeMap&& another) : m_table(std::move(another.m_table))
        {
        }

        ~TreeMap()
        {
            clear();
        }

        NTSTATUS put(const K& key, const V& value)
        {
            V tmp(value);
            return put(key, std::move(tmp));
        }

        NTSTATUS put(const K& key, V&& value)
        {
            Node node(key, std::move(value));
            NTSTATUS status = m_table.insertElement(std::move(node));
            
            if (!NT_SUCCESS(status))
            {
                value = std::move(node.m_value);
            }

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
            return const_cast<TreeMap*>(this)->get(key);
        }

        V* getByIndex(const ULONG index)
        {
            Node* node = m_table.getElement(index);
            if (node)
            {
                return &node->m_value;
            }

            return nullptr;
        }

        const V* getByIndex(const ULONG index) const 
        {
            return const_cast<TreeMap*>(this)->getByIndex(index);
        }

        void clear()
        {
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
            return m_table.deleteElement(Node::fromKey(key));
        }

        // Important! Use only the value returned by other map methods.
        // The usage of the value that doesn't exist in map will lead to undefined behavior.
        bool removeByObject(const V* value)
        {
            Node* node = CONTAINING_RECORD(value, Node, m_value);
            return m_table.deleteElement(*node);
        }

        TreeMap& operator=(_Inout_ TreeMap&& another)
        {
            m_table = std::move(another.m_table);
            return *this;
        }

    private:
        TreeMap(const TreeMap&);
        TreeMap& operator=(const TreeMap&);

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

            Node(Node&& another) : m_key(std::move(another.m_key)), m_value(std::move(another.m_value))
            {
            }

            bool operator<(const Node& another) const
            {
                LessComparer lessComparer;
                return lessComparer(m_key, another.m_key);
            }

            K m_key;
            V m_value;
        };

    private:
        GenericTableAvl<Node, poolType> m_table;
    };
}
