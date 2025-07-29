#pragma once
#include <ranges>
#include <array>

namespace kf
{
    namespace ranges
    {
        template<std::ranges::forward_range Range, size_t N> requires std::ranges::view<Range> && (N > 0) && std::ranges::common_range<Range>
        class adjacent_view : public std::ranges::view_interface<adjacent_view<Range, N>>
        {
        public:
            adjacent_view() requires std::default_initializable<Range> = default;

            explicit adjacent_view(Range base)
                : m_base(std::move(base))
            {
            }

            constexpr auto begin()
            {
                return iterator{ std::ranges::begin(m_base), std::ranges::end(m_base) };
            }

            constexpr auto end()
            {
                return iterator{ std::ranges::end(m_base) };
            }

        private:
            class iterator
            {
            private:
                using range_iterator = std::ranges::iterator_t<Range>;

            public:
                using iterator_category = std::forward_iterator_tag;
                using value_type = void*; // fixme
                using difference_type = std::ptrdiff_t;
                using pointer = void*; // fixme
                using reference = void*; // fixme

                constexpr iterator(range_iterator first, std::ranges::sentinel_t<Range> last)
                {
                    m_current.front() = first;
                    for (size_t i = 1; i < N; ++i)
                    {
                        std::ranges::advance(first, 1, last);
                        m_current[i] = first;
                    }
                }

                constexpr iterator(range_iterator last)
                {
                    if constexpr (!std::ranges::bidirectional_range<Range>)
                    {
                        m_current.fill(last);
                    }
                    else
                    {
                        m_current.back() = last;
                        for (size_t i = 1; i < N; ++i)
                        {
                            std::ranges::advance(last, -1, last);
                            m_current[N - 1 - i] = last;
                        }
                    }
                }

                constexpr auto operator*() const
                {
                    return transformToTuple(m_current);
                }

                constexpr iterator& operator++()
                {
                    for (auto& iter : m_current)
                    {
                        ++iter;
                    }
                    return *this;
                }

                constexpr bool operator==(const iterator& other) const
                {
                    return m_current.back() == other.m_current.back();
                }

            private:
                std::array<range_iterator, N> m_current{};

            private:
                constexpr auto transformToTuple(const decltype(m_current)& arr) const
                {
                    return arrayToTuple(arr, std::make_index_sequence<N>{});
                }

                template <std::size_t... I>
                constexpr auto arrayToTuple(const decltype(m_current)& arr, std::index_sequence<I...>) const
                {
                    return std::tie((*arr[I])...);
                }
            };

        private:
            Range m_base{};
        };

        template<size_t N>
        class adjacent_fn
        {
        public:
            template<std::ranges::viewable_range R> requires std::ranges::forward_range<R>
            auto operator()(R&& r) const
            {
                return adjacent_view<std::views::all_t<R>, N>(std::forward<R>(r));
            }

            // Enables `range | views::adjacent`
            template<std::ranges::viewable_range R> requires std::ranges::forward_range<R>
            friend auto operator|(R&& r, const adjacent_fn& adj)
            {
                return adj(std::forward<R>(r));
            }
        };
    }

    namespace views
    {
        template<size_t N>
        constexpr kf::ranges::adjacent_fn<N> adjacent;

        inline constexpr kf::ranges::adjacent_fn<2> pairwise;
    }
}
