#pragma once
#include <ranges>
#include <array>

namespace kf
{
    template <std::ranges::forward_range Range, size_t N> 
        requires std::ranges::view<Range> && (N > 0) && std::ranges::common_range<Range>
    class AdjacentView : public std::ranges::view_interface<AdjacentView<Range, N>>
    {
    public:
        AdjacentView() requires std::default_initializable<Range> = default;

        explicit AdjacentView(Range base)
            : m_base(std::move(base))
        {
        }

        constexpr auto begin()
        {
            return Iterator{ std::ranges::begin(m_base), std::ranges::end(m_base) };
        }

        constexpr auto end()
        {
            return Iterator{ AsSentinel{}, std::ranges::end(m_base) };
        }

    private:
        struct AsSentinel
        {
            explicit AsSentinel() = default;
        };

        struct Iterator
        {
            using BaseIterator = std::ranges::iterator_t<Range>;
            std::array<BaseIterator, N> m_current{};

            constexpr Iterator(BaseIterator first, std::ranges::sentinel_t<Range> last)
            {
                m_current.front() = first;
                for (size_t i = 1; i < N; ++i)
                {
                    std::ranges::advance(first, 1, last);
                    m_current[i] = first;
                }
            }

            constexpr Iterator(AsSentinel, BaseIterator last)
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

            template <std::size_t... I>
            constexpr auto arrayToTuple(const decltype(m_current)& arr, std::index_sequence<I...>) const
            {
                return std::tie((*arr[I])...);
            }

            constexpr auto transformToTuple(const decltype(m_current)& arr) const
            {
                return arrayToTuple(arr, std::make_index_sequence<N>{});
            }

            constexpr auto operator*() const
            {
                return transformToTuple(m_current);
            }

            constexpr Iterator& operator++()
            {
                for (BaseIterator& iter : m_current)
                {
                    ++iter;
                }
                return *this;
            }

            constexpr bool operator==(const Iterator& other) const
            {
                return m_current.back() == other.m_current.back();
            }
        };

    private:
        Range m_base{};
    };

    template <size_t N>
    class AdjacentFn
    {
    public:
        template <std::ranges::forward_range R>
            requires std::ranges::viewable_range<R>
        auto operator()(R&& r) const
        {
            return AdjacentView<std::views::all_t<R>, N>(std::forward<R>(r));
        }
    };

    template <size_t N>
    constexpr AdjacentFn<N> adjacent;
    inline constexpr AdjacentFn<2> pairwise;
}
