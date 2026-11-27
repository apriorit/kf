#pragma once
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace kf
{
    template<class TElemType, DoubleLinkedListEntry TElemType::* TListEntryMember, bool IsConst>
    class DoubleLinkedListStdIterator
    {
        template<class T, DoubleLinkedListEntry T::* Member, bool Const>
        friend class DoubleLinkedListStdIterator;

        using ListEntry = std::conditional_t<IsConst, const LIST_ENTRY, LIST_ENTRY>;

    public:
        using value_type = TElemType;
        using difference_type = std::ptrdiff_t;
        using pointer = std::conditional_t<IsConst, const TElemType*, TElemType*>;
        using reference = std::conditional_t<IsConst, const TElemType&, TElemType&>;
        using iterator_category = std::bidirectional_iterator_tag;
        using iterator_concept = std::bidirectional_iterator_tag;

        DoubleLinkedListStdIterator() = default;

        explicit DoubleLinkedListStdIterator(ListEntry* current) : m_current(current)
        {
        }

        template<bool OtherConst>
            requires (IsConst && !OtherConst)
        DoubleLinkedListStdIterator(const DoubleLinkedListStdIterator<TElemType, TListEntryMember, OtherConst>& other)
            : m_current(other.m_current)
        {
        }

        reference operator*() const
        {
            return *operator->();
        }

        pointer operator->() const
        {
            using Char = std::conditional_t<IsConst, const char, char>;
            return reinterpret_cast<pointer>(reinterpret_cast<Char*>(m_current) - reinterpret_cast<ULONG_PTR>(&(static_cast<TElemType*>(0)->*TListEntryMember)));
        }

        DoubleLinkedListStdIterator& operator++()
        {
            m_current = m_current->Flink;
            return *this;
        }

        DoubleLinkedListStdIterator operator++(int)
        {
            auto previous = *this;
            ++*this;
            return previous;
        }

        DoubleLinkedListStdIterator& operator--()
        {
            m_current = m_current->Blink;
            return *this;
        }

        DoubleLinkedListStdIterator operator--(int)
        {
            auto previous = *this;
            --*this;
            return previous;
        }

        template<bool OtherConst>
        bool operator==(const DoubleLinkedListStdIterator<TElemType, TListEntryMember, OtherConst>& other) const
        {
            return m_current == other.m_current;
        }

    private:
        ListEntry* m_current = nullptr;
    };
}
