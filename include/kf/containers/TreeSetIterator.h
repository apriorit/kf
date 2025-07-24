#pragma once
#include "GenericTableAvl.h"

namespace kf
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // TreeSetIterator - iterator for set container for NT kernel, inspired by http://docs.oracle.com/javase/7/docs/api/java/util/TreeSet.html

    template<class E, POOL_TYPE poolType>
    class TreeSetIterator
    {
    public:
        TreeSetIterator(GenericTableAvl<E, poolType>& table) : m_table(table), m_restartKey(), m_next()
        {
        }

        bool hasNext()
        {
            if (!m_next)
            {
                m_next = m_table.enumerateWithoutSplaying(m_restartKey);
            }

            return !!m_next;
        }

        E& next()
        {
            if (!m_next)
            {
                m_next = m_table.enumerateWithoutSplaying(m_restartKey);
            }

            auto next = m_next;
            m_next = nullptr;
            return *next;
        }

    private:
        GenericTableAvl<E, poolType>&   m_table;
        PVOID                           m_restartKey;
        E*                              m_next;
    };                                            
}