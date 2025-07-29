#include "pch.h"
#include <kf/stl/backport/ranges/ranges>

SCENARIO("views::adjacent")
{
    GIVEN("array with 4 elements: 1,2,3,4")
    {
        std::array<int, 4> arr = { 1, 2, 3, 4 };

        WHEN("create adjacent view for 1 elements")
        {
            auto view = arr | kf::views::adjacent<1>;

            THEN("view contains 4 tuples: (1)(2)(3)(4)")
            {
                std::array<std::tuple<int>, 4> expected = { { {1}, {2}, {3}, {4} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }

        WHEN("create adjacent view for 2 elements")
        {
            auto view = arr | kf::views::adjacent<2>;

            THEN("view contains 3 tuples: (1,2)(2,3)(3,4)")
            {
                std::array<std::tuple<int, int>, 3> expected = { { {1, 2}, {2, 3}, {3, 4} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }
    }
}