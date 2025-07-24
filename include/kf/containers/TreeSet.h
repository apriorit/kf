#pragma once
#include "GenericTableAvl.h"
#include "TreeSetIterator.h"
#include <functional>
#include <utility>

namespace kf
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // TreeSet - set container for NT kernel, inspired by http://docs.oracle.com/javase/7/docs/api/java/util/TreeSet.html

    template<class E, POOL_TYPE poolType, class LessComparer=std::less<E>>
    class TreeSet
    {
    public:
        TreeSet()
        {
        }

        TreeSet(_Inout_ TreeSet&& another) : m_table(std::move(another.m_table))
        {
        }

        ~TreeSet()
        {
            clear();
        }

        NTSTATUS add(const E& elem)
        {
            return add(E(elem));
        }

        NTSTATUS add(E&& elem)
        {
            return m_table.insertElement(std::move(elem));
        }

        void clear()
        {
            m_table.clear();
        }

        bool contains(const E& elem) const
        {            
            return m_table.lookupElement(elem) != nullptr;
        }

        E* find(const E& elem) const
        {
            return const_cast<E*>(m_table.lookupElement(elem));
        }

        int size() const
        {
            return m_table.number();
        }

        bool isEmpty() const
        {
            return m_table.isEmpty();
        }

        bool remove(const E& elem)
        {
            return m_table.deleteElement(elem);
        }

        TreeSetIterator<E, poolType> iterator()
        {
            return TreeSetIterator<E, poolType>(m_table);
        }

        TreeSet& operator=(_Inout_ TreeSet&& another)
        {
            m_table = std::move(another.m_table);
            return *this;
        }

    private:
        TreeSet(const TreeSet&);
        TreeSet& operator=(const TreeSet&);

    private:
        GenericTableAvl<E, poolType> m_table;
    };
}
