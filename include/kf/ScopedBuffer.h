#pragma once

namespace kf
{
    template<typename T, POOL_TYPE _Pool = PagedPool>
    class [[deprecated("Use kf::vector instead")]] scoped_buffer;
}
