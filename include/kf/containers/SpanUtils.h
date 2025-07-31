#pragma once
#include <span>

namespace kf
{
    using namespace std;

    template<class T, class U>
    inline constexpr span<T> span_cast(span<U> input) noexcept
    {
        return { reinterpret_cast<T*>(input.data()), input.size_bytes() / sizeof(T) };
    }

    template<class T, class U>
    inline constexpr span<T> span_cast(U* data, size_t size) noexcept
    {
        return { reinterpret_cast<T*>(data), size * sizeof(U) / sizeof(T) };
    }

    inline constexpr span<const std::byte> as_bytes(const void* p, size_t size) noexcept
    {
        return { static_cast<const std::byte*>(p), size };
    }

    // TODO: rename to asBytes
    template<class T, size_t N>
    inline constexpr auto as_bytes(const T(&p)[N]) noexcept
    {
        return span<const std::byte, sizeof(T)* N>{ reinterpret_cast<const std::byte*>(p), sizeof(p) };
    }

    inline constexpr span<std::byte> as_writable_bytes(void* p, size_t size) noexcept
    {
        return { static_cast<std::byte*>(p), size };
    }

    // TODO: rename to asWritableBytes
    template<class T, size_t N>
    inline constexpr auto as_writable_bytes(T(&p)[N]) noexcept
    {
        return span<std::byte, sizeof(T) * N>{ reinterpret_cast<std::byte*>(p), sizeof(p) };
    }

    template<class T, size_t dstExtent, size_t srcExtent>
    inline constexpr span<T> copyTruncate(span<T, dstExtent> dst, span<const T, srcExtent> src) noexcept
    {
        //
        // Source can be larger than destination, truncate in such case
        //

        src = src.first(min(src.size(), dst.size()));

        return { dst.begin(), copy(src.begin(), src.end(), dst.begin()) };
    }

    template<class T, size_t dstExtent, size_t srcExtent>
    inline constexpr span<T> copyExact(span<T, dstExtent> dst, span<const T, srcExtent> src) noexcept
    {
        //
        // Source MUST be equal to destination
        //

        if constexpr (dstExtent == dynamic_extent || srcExtent == dynamic_extent)
        {
            if (dst.size() != src.size())
            {
                _Xinvalid_argument("dst.size() != src.size()");
            }
        }
        else
        {
            static_assert(srcExtent == dstExtent);
        }

        return { dst.begin(), copy(src.begin(), src.end(), dst.begin()) };
    }

    template<class T, size_t dstExtent, size_t srcExtent>
    inline constexpr span<T> copy(span<T, dstExtent> dst, span<const T, srcExtent> src) noexcept
    {
        //
        // Source MUST be smaller or equal to destination
        //

        if constexpr (dstExtent == dynamic_extent || srcExtent == dynamic_extent)
        {
            if (dst.size() < src.size())
            {
                _Xinvalid_argument("dst.size() < src.size()");
            }
        }
        else
        {
            static_assert(srcExtent <= dstExtent);
        }

        return { dst.begin(), copy(src.begin(), src.end(), dst.begin()) };
    }

    template<class T, size_t LeftExtent, size_t RightExtent>
    inline constexpr bool equals(span<T, LeftExtent> left, span<T, RightExtent> right) noexcept
    {
        return std::equal(left.begin(), left.end(), right.begin(), right.end());
    }

    template<class T>
    inline constexpr ptrdiff_t indexOf(span<T> input, typename span<T>::const_reference elem, ptrdiff_t fromIndex = 0) noexcept
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
    inline constexpr span<T> split(span<T> input, typename span<T>::const_reference separator, _Inout_ ptrdiff_t& fromIndex) noexcept
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
    inline constexpr T atOrDefault(span<T> input, size_t index, convertible_to<T> auto defaultValue) noexcept
    {
        return input.size() > index ? input[index] : defaultValue;
    }
}
