#pragma once
#include <span>

namespace kf
{
    template<class T, class U>
    inline constexpr std::span<T> span_cast(std::span<U> input) noexcept
    {
        return { reinterpret_cast<T*>(input.data()), input.size_bytes() / sizeof(T) };
    }

    template<class T, class U>
    inline constexpr std::span<T> span_cast(U* data, size_t size) noexcept
    {
        return { reinterpret_cast<T*>(data), size * sizeof(U) / sizeof(T) };
    }

    inline constexpr std::span<const std::byte> as_bytes(const void* p, size_t size) noexcept
    {
        return { static_cast<const std::byte*>(p), size };
    }

    // TODO: rename to asBytes
    template<class T, size_t N>
    inline constexpr auto as_bytes(const T(&p)[N]) noexcept
    {
        return std::span<const std::byte, sizeof(T)* N>{ reinterpret_cast<const std::byte*>(p), sizeof(p) };
    }

    inline constexpr std::span<std::byte> as_writable_bytes(void* p, size_t size) noexcept
    {
        return { static_cast<std::byte*>(p), size };
    }

    // TODO: rename to asWritableBytes
    template<class T, size_t N>
    inline constexpr auto as_writable_bytes(T(&p)[N]) noexcept
    {
        return std::span<std::byte, sizeof(T) * N>{ reinterpret_cast<std::byte*>(p), sizeof(p) };
    }

    template<class T, size_t dstExtent, size_t srcExtent>
    inline constexpr std::span<T> copyTruncate(std::span<T, dstExtent> dst, std::span<const T, srcExtent> src) noexcept
    {
        //
        // Source can be larger than destination, truncate in such case
        //

        src = src.first(min(src.size(), dst.size()));

        return { dst.begin(), copy(src.begin(), src.end(), dst.begin()) };
    }

    template<class T, size_t dstExtent, size_t srcExtent>
    inline constexpr std::span<T> copyExact(std::span<T, dstExtent> dst, std::span<const T, srcExtent> src) noexcept
    {
        //
        // Source MUST be equal to destination
        //

        if constexpr (dstExtent == std::dynamic_extent || srcExtent == std::dynamic_extent)
        {
            if (dst.size() != src.size())
            {
                std::_Xinvalid_argument("dst.size() != src.size()");
            }
        }
        else
        {
            static_assert(srcExtent == dstExtent);
        }

        return { dst.begin(), copy(src.begin(), src.end(), dst.begin()) };
    }

    template<class T, size_t dstExtent, size_t srcExtent>
    inline constexpr std::span<T> copy(std::span<T, dstExtent> dst, std::span<const T, srcExtent> src) noexcept
    {
        //
        // Source MUST be smaller or equal to destination
        //

        if constexpr (dstExtent == std::dynamic_extent || srcExtent == std::dynamic_extent)
        {
            if (dst.size() < src.size())
            {
                std::_Xinvalid_argument("dst.size() < src.size()");
            }
        }
        else
        {
            static_assert(srcExtent <= dstExtent);
        }

        return { dst.begin(), copy(src.begin(), src.end(), dst.begin()) };
    }

    template<class T, size_t LeftExtent, size_t RightExtent>
    inline constexpr bool equals(std::span<T, LeftExtent> left, std::span<T, RightExtent> right) noexcept
    {
        return std::equal(left.begin(), left.end(), right.begin(), right.end());
    }

    template<class T>
    inline constexpr ptrdiff_t indexOf(std::span<T> input, typename std::span<T>::const_reference elem, ptrdiff_t fromIndex = 0) noexcept
    {
        for (auto i = fromIndex; i < ssize(input); ++i)
        {
            if (input[i] == elem)
            {
                return i;
            }
        }

        return -1;
    }

    template<class T>
    inline constexpr std::span<T> split(std::span<T> input, typename std::span<T>::const_reference separator, _Inout_ ptrdiff_t& fromIndex) noexcept
    {
        auto originalFromIndex = fromIndex;

        fromIndex = indexOf(input, separator, fromIndex);
        if (fromIndex < 0)
        {
            return input.subspan(originalFromIndex);
        }

        const auto count = fromIndex - originalFromIndex;
        ++fromIndex;

        return input.subspan(originalFromIndex, count);
    }

    template<class T>
    inline constexpr T atOrDefault(std::span<T> input, size_t index, std::convertible_to<T> auto defaultValue) noexcept
    {
        return input.size() > index ? input[index] : defaultValue;
    }
}
