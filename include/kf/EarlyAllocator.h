#pragma once

namespace kf
{

    template<class T = std::byte>
    class EarlyAllocator
    {
    public:
        static_assert(!std::is_const_v<T>, "The C++ Standard forbids containers of const elements because allocator<const T> is ill-formed.");

        using value_type = T;
        using size_type = std::size_t;
        using difference_type = std::ptrdiff_t;
        using propagate_on_container_move_assignment = std::true_type;

        constexpr EarlyAllocator() noexcept = default;

        constexpr EarlyAllocator(const EarlyAllocator& other) noexcept = default;

        template<class Y>
        friend class EarlyAllocator;

        template<class Y>
        constexpr EarlyAllocator(const EarlyAllocator<Y>& other) noexcept : m_ptr(reinterpret_cast<T*>(other.m_ptr)), m_size(other.m_size)
        {
        }

        ~EarlyAllocator() noexcept = default;
        EarlyAllocator& operator=(const EarlyAllocator&) noexcept = default;

        template<POOL_TYPE poolType>
        T* initialize(const std::size_t count) noexcept
        {
            if (m_ptr || m_size)
            {
                _Xinvalid_argument("m_ptr || m_size");
            }

            m_size = count * sizeof(T);
            m_ptr = static_cast<T*>(operator new(m_size, poolType));

            return m_ptr;
        }

        void deallocate(const T* ptr, const std::size_t count) noexcept
        {
            if (ptr != m_ptr || count * sizeof(T) > m_size)
            {
                _Xinvalid_argument("ptr != m_ptr || count * sizeof(T) > m_size");
            }

            operator delete(m_ptr);
            m_ptr = nullptr;
            m_size = 0;
        }

        _NODISCARD T* allocate(const std::size_t count) noexcept
        {
            if (!m_ptr || count * sizeof(T) > m_size)
            {
                _Xinvalid_argument("!m_ptr || count * sizeof(T) > m_size");
            }

            return m_ptr;
        }

    private:
        T* m_ptr = nullptr;
        std::size_t m_size = 0;
    };
}
