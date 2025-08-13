#pragma once

namespace kf
{
    struct DoubleLinkedListEntry : public LIST_ENTRY
    {
        DoubleLinkedListEntry()
        {
            ::InitializeListHead(this);
        }

        DoubleLinkedListEntry(const DoubleLinkedListEntry& other) = delete;

        DoubleLinkedListEntry(DoubleLinkedListEntry&& other)
        {
            *this = std::move(other);
        }

        ~DoubleLinkedListEntry()
        {
            ASSERT(::IsListEmpty(this));
        }

        DoubleLinkedListEntry& operator=(const DoubleLinkedListEntry& other) = delete;

        DoubleLinkedListEntry& operator=(DoubleLinkedListEntry&& other)
        {
            if (this != &other)
            {
                ::InsertHeadList(&other, this);
                ::RemoveEntryList(&other);
                ::InitializeListHead(&other);
            }

            return *this;
        }

        bool isEmpty() const
        {
            return ::IsListEmpty(this);
        }
    };
}
