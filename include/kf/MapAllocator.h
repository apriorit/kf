#pragma once
#include <kf/stl/memory>

namespace kf
{
    struct AllocatorState
    {
        std::unique_ptr<char[]> buffer;
        size_t size;
    };

    template<typename T, POOL_TYPE poolType>
    class MapAllocator
    {
    public:
        using value_type = T;

        MapAllocator() = default;

        // Needed because allocator has extra parameter for POOL_TYPE
        template <typename U>
        struct rebind
        {
            using other = MapAllocator<U, poolType>;
        };

        friend class MapAllocator;

        template<typename U>
        MapAllocator(const MapAllocator<U, poolType>& other)
            : m_state(other.m_state)
        {
        }

        [[nodiscard]] bool initialize()
        {
            m_state = make_shared<AllocatorState, poolType>();
            return m_state != nullptr;
        }

        T* allocate(std::size_t n)
        {
            if (!m_state || !m_state->buffer)
            {
                return nullptr;
            }

            if (m_state->size < n)
            {
                return nullptr;
            }

            auto buffer = reinterpret_cast<T*>(m_state->buffer.release());
            m_state->size = 0;
            return buffer;
        }

        void deallocate(T*, std::size_t)
        {
            m_state->buffer.reset();
            m_state->size = 0;
        }

        [[nodiscard]] bool prepareMemory(std::size_t size)
        {
            if (!m_state)
            {
                return false;
            }

            if (m_state->buffer)
            {
                return true;
            }

            m_state->buffer.reset(new(poolType) char[size]);
            if (!m_state->buffer)
            {
                return false;
            }
            m_state->size = size;

            return true;
        }

    private:
        std::shared_ptr<AllocatorState> m_state;
    };
}
