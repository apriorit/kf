#pragma once
#include <array>

namespace kf
{
    using namespace std;

    template<typename... Ts>
    inline constexpr std::array<std::byte, sizeof...(Ts)> makeArrayOfBytes(Ts&&... args) noexcept
    {
        return { static_cast<std::byte>(args)... };
    }
}
