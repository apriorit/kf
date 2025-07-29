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

        WHEN("create adjacent view for 3 elements")
        {
            auto view = arr | kf::views::adjacent<3>;

            THEN("view contains 2 tuples: (1,2,3)(2,3,4)")
            {
                std::array<std::tuple<int, int, int>, 2> expected = { { {1, 2, 3}, {2, 3, 4} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }

        WHEN("create adjacent view for 4 elements")
        {
            auto view = arr | kf::views::adjacent<4>;

            THEN("view contains 1 tuple: (1,2,3,4)")
            {
                std::array<std::tuple<int, int, int, int>, 1> expected = { { {1, 2, 3, 4} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }
    }
}

SCENARIO("views::pairwise")
{
    GIVEN("array with 5 elements: 10,20,30,40,50")
    {
        std::array<int, 5> arr = { 10, 20, 30, 40, 50 };

        WHEN("create pairwise view")
        {
            auto view = arr | kf::views::pairwise;

            THEN("view contains 4 pairs: (10,20)(20,30)(30,40)(40,50)")
            {
                std::array<std::tuple<int, int>, 4> expected = { { {10, 20}, {20, 30}, {30, 40}, {40, 50} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }
    }
}

SCENARIO("views::adjacent edge cases")
{
    GIVEN("array with 1 element: 42")
    {
        std::array<int, 1> arr = { 42 };

        WHEN("create adjacent view for 1 element")
        {
            auto view = arr | kf::views::adjacent<1>;

            THEN("view contains 1 tuple: (42)")
            {
                std::array<std::tuple<int>, 1> expected = { { {42} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }
    }

    GIVEN("array with 2 elements: 5,7")
    {
        std::array<int, 2> arr = { 5, 7 };

        WHEN("create adjacent view for 2 elements")
        {
            auto view = arr | kf::views::adjacent<2>;

            THEN("view contains 1 tuple: (5,7)")
            {
                std::array<std::tuple<int, int>, 1> expected = { { {5, 7} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }

        WHEN("create pairwise view")
        {
            auto view = arr | kf::views::pairwise;

            THEN("view contains 1 pair: (5,7)")
            {
                std::array<std::tuple<int, int>, 1> expected = { { {5, 7} } };

                REQUIRE(std::equal(view.begin(), view.end(), expected.begin(), expected.end()));
            }
        }
    }

    GIVEN("array with 2 elements but adjacent window of 3")
    {
        std::array<int, 2> arr = { 10, 20 };

        WHEN("create adjacent view for 3 elements")
        {
            auto view = arr | kf::views::adjacent<3>;

            THEN("view is empty since window is larger than input")
            {
                REQUIRE(view.begin() == view.end());
            }
        }
    }

    GIVEN("array with 1 element but adjacent window of 2")
    {
        std::array<int, 1> arr = { 99 };

        WHEN("create pairwise view")
        {
            auto view = arr | kf::views::pairwise;

            THEN("view is empty since window is larger than input")
            {
                REQUIRE(view.begin() == view.end());
            }
        }
    }
}