#pragma once
#include <algorithm>

namespace kf
{
    template<class ForwardIt, class T>
    constexpr ForwardIt binary_search_it(ForwardIt first, ForwardIt last, const T& value)
    {
        first = std::lower_bound(first, last, value);
        if (!(first == last) && !(value < *first))
        {
            return first;
        }
        return last;
    }

    template<class ForwardIt, class T, class Compare>
    constexpr ForwardIt binary_search_it(ForwardIt first, ForwardIt last, const T& value, Compare comp)
    {
        first = std::lower_bound(first, last, value, comp);
        if (!(first == last) && !(comp(value, *first)))
        {
            return first;
        }
        return last;
    }
}
