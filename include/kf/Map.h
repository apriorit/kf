#pragma once
#include <kf/MapAllocator.h>
#include <map>
#include <optional>

#if _ITERATOR_DEBUG_LEVEL > 0
#error "_ITERATOR_DEBUG_LEVEL must not be greater than 0"
#endif

namespace kf
{
    template<typename KeyType, typename ValueType, POOL_TYPE poolType, typename LessComparer = std::less<KeyType>>
    class Map
    {
    public:
        using AllocatorType = MapAllocator<std::pair<const KeyType, ValueType>, poolType>;
        using MapType = std::map<KeyType, ValueType, LessComparer, AllocatorType>;
        using Iterator = MapType::iterator;
        using ConstIterator = MapType::const_iterator;
        using SizeType = MapType::size_type;

    public:
        Map() = default;

        Map(const Map&) = delete;
        Map& operator=(const Map&) = delete;

        Map(Map&& other)
            : m_internalMap(std::move(other.m_internalMap))
        {
        }

        Map& operator=(Map&& other)
        {
            m_internalMap = std::move(other.m_internalMap);
            return *this;
        }

        NTSTATUS init()
        {
            AllocatorType allocator;
            if (!allocator.init())
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }
            
            // Prepare memory for head node
            if (!allocator.prepareMemory(kNodeSize))
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            m_internalMap = make_unique<MapType, poolType>(allocator);
            if (!m_internalMap)
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            return STATUS_SUCCESS;
        }

        std::optional<std::reference_wrapper<ValueType>> operator[](KeyType&& key)
        {
            m_internalMap->get_allocator().prepareMemory(kNodeSize);
            return std::ref((*m_internalMap)[key]);
        }

        template <class... ValuesType>
        std::optional<std::pair<Iterator, bool>> emplace(ValuesType&&... values)
        {
            m_internalMap->get_allocator().prepareMemory(kNodeSize);
            return m_internalMap->emplace(std::forward<ValuesType>(values)...);
        }

        [[nodiscard]] Iterator find(const KeyType& key)
        {
            return m_internalMap->find(key);
        }

        [[nodiscard]] ConstIterator find(const KeyType& key) const
        {
            return m_internalMap->find(key);
        }

#if _HAS_CXX20
        [[nodiscard]] bool contains(const KeyType& key) const
        {
            return m_internalMap->contains(key);
        }
#endif

        void clear() noexcept
        {
            m_internalMap->clear();
        }

        Iterator erase(ConstIterator where) noexcept
        {
            return m_internalMap->erase(where);
        }

        Iterator erase(ConstIterator first, ConstIterator last) noexcept
        {
            return m_internalMap->erase(first, last);
        }

        SizeType erase(const KeyType& key) noexcept
        {
            return m_internalMap->erase(key);
        }

        [[nodiscard]] Iterator begin() noexcept
        {
            return m_internalMap->begin();
        }

        [[nodiscard]] ConstIterator begin() const noexcept
        {
            return m_internalMap->begin();
        }

        [[nodiscard]] Iterator end() noexcept
        {
            return m_internalMap->end();
        }

        [[nodiscard]] ConstIterator end() const noexcept
        {
            return m_internalMap->end();
        }

        [[nodiscard]] Iterator upper_bound(const KeyType& key)
        {
            return m_internalMap->upper_bound(key);
        }

        [[nodiscard]] ConstIterator upper_bound(const KeyType& key) const
        {
            return m_internalMap->upper_bound(key);
        }

        bool empty() const noexcept
        {
            return m_internalMap->empty();
        }

        SizeType size() const noexcept
        {
            return m_internalMap->size();
        }

    private:
        static constexpr size_t kNodeSize = sizeof(MapType::_Alnode_traits::value_type);
        std::unique_ptr<MapType> m_internalMap;
    };
}
