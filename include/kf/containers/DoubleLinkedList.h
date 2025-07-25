#pragma once
#include "DoubleLinkedListEntry.h"
#include "DoubleLinkedListIterator.h"
#include "DoubleLinkedListDescendingIterator.h"
#include "DoubleLinkedListConstIterator.h"

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // DoubleLinkedList

    template<class TElemType, DoubleLinkedListEntry TElemType::* TListEntryMember>
    class DoubleLinkedList
    {
    public:
        typedef DoubleLinkedListIterator<TElemType, TListEntryMember> Iterator;
        typedef DoubleLinkedListConstIterator<TElemType, TListEntryMember> ConstIterator;
        typedef DoubleLinkedListDescendingIterator<TElemType, TListEntryMember> DescendingIterator;

    public:
        DoubleLinkedList()
        {
        }

        ~DoubleLinkedList()
        {
            clear();
        }

        DoubleLinkedList(_Inout_ DoubleLinkedList&& other)
        {
            *this = std::move(other);
        }

        DoubleLinkedList& operator=(DoubleLinkedList&& other)
        {
            if (this != &other)
            {
                if (!other.isEmpty())
                {
                    LIST_ENTRY* next = other.m_head.Flink;
                    LIST_ENTRY* prev = other.m_head.Blink;

                    next->Blink = &m_head;
                    prev->Flink = &m_head;

                    m_head.Flink = next;
                    m_head.Blink = prev;

                    ::InitializeListHead(&other.m_head);
                }
                else
                {
                    ::InitializeListHead(&m_head);
                }
            }

            return *this;

        }

        void addFirst(_Inout_ TElemType& listElement)
        {
            auto listEntry = toListEntry(listElement);
            ASSERT(::IsListEmpty(listEntry));

            ::InsertHeadList(&m_head, listEntry);
        }

        void addLast(_Inout_ TElemType& listElement)
        {
            auto listEntry = toListEntry(listElement);
            ASSERT(::IsListEmpty(listEntry));

            ::InsertTailList(&m_head, listEntry);
        }

        static void addBefore(_Inout_ TElemType& existingListElement, _Inout_ TElemType& newListElement)
        {
            auto newListEntry = toListEntry(newListElement);
            ASSERT(::IsListEmpty(newListEntry));

            auto existingListEntry = toListEntry(existingListElement);
            ASSERT(!::IsListEmpty(existingListEntry));

            ::InsertTailList(existingListEntry, newListEntry);
        }

        static void addAfter(_Inout_ TElemType& existingListElement, _Inout_ TElemType& newListElement)
        {
            auto newListEntry = toListEntry(newListElement);
            ASSERT(::IsListEmpty(newListEntry));

            auto existingListEntry = toListEntry(existingListElement);
            ASSERT(!::IsListEmpty(existingListEntry));

            ::InsertHeadList(existingListEntry, newListEntry);
        }

        TElemType* removeFirst()
        {
            LIST_ENTRY* const listEntry = ::RemoveHeadList(&m_head);
            ::InitializeListHead(listEntry);

            if (listEntry == &m_head)
            {
                return nullptr;
            }

            return fromListEntry(listEntry);
        }

        bool isEmpty() const
        {
            return !!::IsListEmpty(&m_head);
        }

        int size() const
        {
            int size = 0;

            for (auto current = m_head.Flink; current != &m_head; current = current->Flink)
            {
                ++size;
            }

            return size;
        }

        int indexOf(const TElemType& listElement) const
        {
            int index = 0;
            auto listEntry = toListEntry(listElement);

            for (auto current = m_head.Flink; current != &m_head; current = current->Flink)
            {
                if (current == listEntry)
                {
                    return index;
                }

                ++index;
            }

            return -1;
        }

        bool contains(const TElemType& listElement) const
        {
            return indexOf(listElement) >= 0;
        }

        bool remove(_Inout_ TElemType& listElement)
        {
            auto listEntry = toListEntry(listElement);

            if (::IsListEmpty(listEntry))
            {
                return false;
            }

            ::RemoveEntryList(listEntry);
            ::InitializeListHead(listEntry);
            return true;
        }

        void clear()
        {
            auto it = iterator();
            while (it.hasNext())
            {
                it.next(); // must be called before you can call it.remove()
                it.remove();
            }
        }

        Iterator iterator()
        {
            return Iterator(&m_head);
        }

        ConstIterator iterator() const
        {
            return ConstIterator(&m_head);
        }

        Iterator iterator(_Inout_ TElemType& listElement)
        {
            auto listEntry = toListEntry(listElement);
            ASSERT(!::IsListEmpty(listEntry));

            return Iterator(&m_head, listEntry);
        }

        DescendingIterator descendingIterator()
        {
            return DescendingIterator(&m_head);
        }

        DescendingIterator descendingIterator(_Inout_ TElemType& listElement)
        {
            auto listEntry = toListEntry(listElement);
            ASSERT(!::IsListEmpty(listEntry));

            return DescendingIterator(&m_head, listEntry);
        }

    private:
        static TElemType* fromListEntry(_In_ LIST_ENTRY* listEntry)
        {
            // Implementation of CONTAINING_RECORD macro
            return reinterpret_cast<TElemType*>(reinterpret_cast<char*>(listEntry) - reinterpret_cast<ULONG_PTR>(&(static_cast<TElemType*>(0)->*TListEntryMember)));
        }

        static const LIST_ENTRY* toListEntry(_In_ const TElemType& listElement)
        {
            return &(listElement.*TListEntryMember);
        }

        static LIST_ENTRY* toListEntry(_In_ TElemType& listElement)
        {
            return &(listElement.*TListEntryMember);
        }

    private:
        DoubleLinkedList(const DoubleLinkedList&);
        DoubleLinkedList& operator=(const DoubleLinkedList&);

    private:
        DoubleLinkedListEntry m_head;
    };
}
