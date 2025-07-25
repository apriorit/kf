#pragma once
#include "DoubleLinkedListEntry.h"

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // Iterator

    template<class TElemType, DoubleLinkedListEntry TElemType::* TListEntryMember>
    class DoubleLinkedListConstIterator
    {
    public:
        DoubleLinkedListConstIterator(const LIST_ENTRY* head) : m_head(head), m_current(m_head)
        {
        }

        DoubleLinkedListConstIterator(const LIST_ENTRY* head, const LIST_ENTRY* current) : m_head(head), m_current(current)
        {
        }

        bool hasNext() const
        {
            return m_current->Flink != m_head;
        }

        const TElemType* next()
        {
            ASSERT(hasNext());
            m_current = m_current->Flink;

            return fromListEntry(m_current);
        }

    private:
        static const TElemType* fromListEntry(_In_ const LIST_ENTRY* listEntry)
        {
            // Implementation of CONTAINING_RECORD macro
            return reinterpret_cast<const TElemType*>(reinterpret_cast<const char*>(listEntry) - reinterpret_cast<ULONG_PTR>(&(static_cast<TElemType*>(0)->*TListEntryMember)));
        }

    private:
        const LIST_ENTRY* m_head;
        const LIST_ENTRY* m_current;
    };
}
