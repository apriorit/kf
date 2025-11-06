#pragma once
#include <array>

namespace kf
{

    template<typename... Ts>
    inline constexpr std::array<std::byte, sizeof...(Ts)> makeArrayOfBytes(Ts&&... args) noexcept
    {
        return { static_cast<std::byte>(args)... };
    }
}
