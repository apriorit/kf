#pragma once
#include "DoubleLinkedListEntry.h"

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // Iterator

    template<class TElemType, DoubleLinkedListEntry TElemType::* TListEntryMember>
    class DoubleLinkedListIterator
    {
    public:
        DoubleLinkedListIterator(_Inout_ LIST_ENTRY* head) : m_head(head), m_current(m_head)
        {
        }

        DoubleLinkedListIterator(_Inout_ LIST_ENTRY* head, _Inout_ LIST_ENTRY* current) : m_head(head), m_current(current)
        {
        }

        bool hasNext() const
        {
            return m_current->Flink != m_head;
        }

        TElemType* next()
        {
            ASSERT(hasNext());
            m_current = m_current->Flink;

            return fromListEntry(m_current);
        }

        void remove()
        {
            ASSERT(m_current != m_head);
            m_current = m_current->Blink;

            auto listEntry = m_current->Flink;

            ::RemoveEntryList(listEntry);
            ::InitializeListHead(listEntry);
        }

    private:
        static TElemType* fromListEntry(_In_ LIST_ENTRY* listEntry)
        {
            // Implementation of CONTAINING_RECORD macro
            return reinterpret_cast<TElemType*>(reinterpret_cast<char*>(listEntry) - reinterpret_cast<ULONG_PTR>(&(static_cast<TElemType*>(0)->*TListEntryMember)));
        }

    private:
        LIST_ENTRY* m_head;
        LIST_ENTRY* m_current;
    };
}