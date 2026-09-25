#include "pch.h"
#include <kf/algorithm/timsort.h>

SCENARIO("timsort::binary_insertion_sort")
{
    GIVEN("An empty array")
    {
        std::array<int, 0> empty{};

        WHEN("The array is sorted")
        {
            timsort::binary_insertion_sort(empty.data(), empty.size());

            THEN("Sorting completes")
            {
                REQUIRE(empty.empty());
            }
        }
    }

    GIVEN("A single-element array")
    {
        std::array<int, 1> single = { 42 };

        WHEN("The array is sorted")
        {
            timsort::binary_insertion_sort(single.data(), single.size());

            THEN("Its value is unchanged")
            {
                REQUIRE(single[0] == 42);
            }
        }
    }

    GIVEN("Values in descending order with duplicates")
    {
        std::array<int, 9> values = { 9, 7, 7, 5, 3, 3, 1, 0, -2 };
        constexpr std::array<int, 9> expected = { -2, 0, 1, 3, 3, 5, 7, 7, 9 };

        WHEN("The values are sorted")
        {
            timsort::binary_insertion_sort(values.data(), values.size());

            THEN("They are in ascending order")
            {
                REQUIRE(values == expected);
            }
        }
    }

    GIVEN("An array of std::pair values")
    {
        std::array<std::pair<int, int>, 6> values =
        {{
            { 3, 0 }, { 1, 1 }, { 2, 2 }, { 1, 3 }, { 3, 4 }, { 2, 5 }
        }};
        constexpr std::array<std::pair<int, int>, 6> expected =
        {{
            { 1, 1 }, { 1, 3 }, { 2, 2 }, { 2, 5 }, { 3, 0 }, { 3, 4 }
        }};

        WHEN("The values are sorted")
        {
            timsort::binary_insertion_sort(values.data(), values.size());

            THEN("They are ordered lexicographically")
            {
                REQUIRE(values == expected);
            }
        }
    }
}

SCENARIO("timsort::tim_sort")
{
    GIVEN("An empty array")
    {
        std::array<int, 0> empty{};

        WHEN("The array is sorted")
        {
            const NTSTATUS status = timsort::tim_sort(empty);

            THEN("Sorting succeeds")
            {
                REQUIRE(status == STATUS_SUCCESS);
            }
        }
    }

    GIVEN("A single-element array")
    {
        std::array<int, 1> single = { 42 };

        WHEN("The array is sorted")
        {
            const NTSTATUS status = timsort::tim_sort(single);

            THEN("Sorting succeeds without changing its value")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(single[0] == 42);
            }
        }
    }

    GIVEN("An input containing many short, alternating runs")
    {
        std::array<int, 128> values =
        {
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2, 6, 10, 3, 7,
            0, 4, 8, 1, 5, 9, 2
        };
        constexpr std::array<int, 128> expected =
        {
            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
            1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
            2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
            3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
            4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
            5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
            6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
            7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
            8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
            9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
            10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10
        };

        WHEN("The values are sorted")
        {
            const NTSTATUS status = timsort::tim_sort(values);

            THEN("All runs are merged into a sorted sequence")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(values == expected);
            }
        }
    }

    GIVEN("An array of std::pair values")
    {
        std::array<std::pair<int, int>, 6> values =
        {{
            { 3, 0 }, { 1, 1 }, { 2, 2 }, { 1, 3 }, { 3, 4 }, { 2, 5 }
        }};
        constexpr std::array<std::pair<int, int>, 6> expected =
        {{
            { 1, 1 }, { 1, 3 }, { 2, 2 }, { 2, 5 }, { 3, 0 }, { 3, 4 }
        }};

        WHEN("The values are sorted")
        {
            const NTSTATUS status = timsort::tim_sort(values);

            THEN("They are ordered lexicographically")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(values == expected);
            }
        }
    }

    GIVEN("An already sorted input in ascending order")
    {
        std::array<int, 80> values =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
            20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
            30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
            40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
            50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
            60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
            70, 71, 72, 73, 74, 75, 76, 77, 78, 79
        };
        constexpr std::array<int, 80> expected =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
            20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
            30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
            40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
            50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
            60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
            70, 71, 72, 73, 74, 75, 76, 77, 78, 79
        };

        WHEN("The values are sorted")
        {
            const NTSTATUS status = timsort::tim_sort(values);

            THEN("They remain in ascending order")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(values == expected);
            }
        }
    }

    GIVEN("An input in descending order")
    {
        std::array<int, 80> values =
        {
            79, 78, 77, 76, 75, 74, 73, 72, 71, 70,
            69, 68, 67, 66, 65, 64, 63, 62, 61, 60,
            59, 58, 57, 56, 55, 54, 53, 52, 51, 50,
            49, 48, 47, 46, 45, 44, 43, 42, 41, 40,
            39, 38, 37, 36, 35, 34, 33, 32, 31, 30,
            29, 28, 27, 26, 25, 24, 23, 22, 21, 20,
            19, 18, 17, 16, 15, 14, 13, 12, 11, 10,
            9, 8, 7, 6, 5, 4, 3, 2, 1, 0
        };
        constexpr std::array<int, 80> expected =
        {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
            10, 11, 12, 13, 14, 15, 16, 17, 18, 19,
            20, 21, 22, 23, 24, 25, 26, 27, 28, 29,
            30, 31, 32, 33, 34, 35, 36, 37, 38, 39,
            40, 41, 42, 43, 44, 45, 46, 47, 48, 49,
            50, 51, 52, 53, 54, 55, 56, 57, 58, 59,
            60, 61, 62, 63, 64, 65, 66, 67, 68, 69,
            70, 71, 72, 73, 74, 75, 76, 77, 78, 79
        };

        WHEN("The values are sorted")
        {
            const NTSTATUS status = timsort::tim_sort(values);

            THEN("They are reversed into ascending order")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(values == expected);
            }
        }
    }
}
