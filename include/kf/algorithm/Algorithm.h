#pragma once
#include <algorithm>

namespace kf
{
    template< class ForwardIt, class T >
    constexpr ForwardIt binary_search_it(ForwardIt first, ForwardIt last, const T& value)
    {
        first = std::lower_bound(first, last, value);
        if (!(first == last) && !(value < *first))
        {
            return first;
        }
        return last;
    }
}
