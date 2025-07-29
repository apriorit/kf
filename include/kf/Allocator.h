#pragma once

#include "stl/new"

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // Allocator

    template <class T, POOL_TYPE PoolType>
    class Allocator
    {
    public:
        static_assert(!std::is_const_v<T>, "The C++ Standard forbids containers of const elements because allocator<const T> is ill-formed.");

        using value_type = T;
        using size_type = size_t;
        using difference_type = ptrdiff_t;
        using propagate_on_container_swap = std::true_type;

        constexpr Allocator() noexcept = default;

        Allocator(const Allocator&) noexcept = default;

        template <typename Other>
        constexpr Allocator(const Allocator<Other, PoolType>&) noexcept {}

        constexpr void deallocate(T* const p, size_t) noexcept
        {
            operator delete(p);
        }

        [[nodiscard]] constexpr T* allocate(const size_t count) noexcept
        {
            return static_cast<value_type*>(operator new(count * sizeof(T), PoolType));
        }

        template <typename Other>
        struct rebind
        {
            using other = Allocator<Other, PoolType>;
        };
    };
}
