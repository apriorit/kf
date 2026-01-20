#include "pch.h"
#undef __cpp_lib_to_underlying // pretend that the standard library doesn't provide to_underlying
#include <kf/stl/backport/utility/detail/to_underlying.h>

enum class Color : uint8_t
{
    Red = 1,
    Green = 2,
    Blue = 3,
};

static_assert(kf::to_underlying(Color::Blue) == 3);
static_assert(std::is_same_v<decltype(kf::to_underlying(Color::Blue)), uint8_t>);