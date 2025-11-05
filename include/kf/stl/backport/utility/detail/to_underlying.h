#pragma once
#include <utility>

#if !defined(__cpp_lib_to_underlying) // backport for older compilers

namespace kf
{
    template<typename Enum>
    constexpr auto to_underlying(Enum e) noexcept
    {
        return static_cast<typename std::underlying_type_t<Enum>>(e);
    }
}

#endif