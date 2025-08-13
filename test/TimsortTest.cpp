#include "pch.h"
#include <kf/algorithm/timsort.h>
#include <string_view>

using namespace timsort::detail;

SCENARIO("cmp function")
{
    GIVEN("Two integers")
    {
        THEN("5 < 10")
        {
            REQUIRE(cmp(5, 10) == -1);
        }
        THEN("10 > 5")
        {
            REQUIRE(cmp(10, 5) == 1);
        }
        THEN("10 == 10")
        {
            REQUIRE(cmp(10, 10) == 0);
        }
    }

    GIVEN("Two strings")
    {
        THEN("'aaa' < 'bbb'")
        {
            REQUIRE(cmp(std::wstring_view(L"aaa"), std::wstring_view(L"bbb")) == -1);
        }
        THEN("'bbb' > 'aaa'")
        {
            REQUIRE(cmp(std::wstring_view(L"bbb"), std::wstring_view(L"aaa")) == 1);
        }
        THEN("'ccc' == 'ccc'")
        {
            REQUIRE(cmp(std::wstring_view(L"ccc"), std::wstring_view(L"ccc")) == 0);
        }
    }
}

SCENARIO("clzll function")
{
    GIVEN("A zero input")
    {
        uint64_t input = 0; // Binary: 0000000000000000000000000000000000000000000000000000000000000000

        THEN("The result should be 64")
        {
            REQUIRE(clzll(input) == 64);
        }
    }

    GIVEN("A 64-bit integer with only the most significant bit set")
    {
        uint64_t input = 0x8000000000000000; // Binary: 1000000000000000000000000000000000000000000000000000000000000000

        THEN("The result should be 0")
        {
            REQUIRE(clzll(input) == 0);
        }
    }

    GIVEN("A 64-bit integer with the least significant bit set")
    {
        uint64_t input = 0x0000000000000001; // Binary: 0000000000000000000000000000000000000000000000000000000000000001

        THEN("The result should be 63")
        {
            REQUIRE(clzll(input) == 63);
        }
    }

    GIVEN("A 64-bit integer with the top two bits set")
    {
        uint64_t input = 0xC000000000000000; // Binary: 1100000000000000000000000000000000000000000000000000000000000000

        THEN("The result should be 0")
        {
            REQUIRE(clzll(input) == 0);
        }
    }

    GIVEN("A 64-bit integer with the top bit cleared and the second bit set")
    {
        uint64_t input = 0x4000000000000000; // Binary: 0100000000000000000000000000000000000000000000000000000000000000

        THEN("The result should be 1")
        {
            REQUIRE(clzll(input) == 1);
        }
    }

    GIVEN("A 64-bit integer with all bits set")
    {
        uint64_t input = 0xFFFFFFFFFFFFFFFF; // Binary: 1111111111111111111111111111111111111111111111111111111111111111

        THEN("The result should be 0")
        {
            REQUIRE(clzll(input) == 0);
        }
    }

    GIVEN("A 64-bit integer with the top 32 bits cleared")
    {
        uint64_t input = 0x00000000FFFFFFFF; // Binary: 0000000000000000000000000000000011111111111111111111111111111111

        THEN("The result should be 32")
        {
            REQUIRE(clzll(input) == 32);
        }
    }

    GIVEN("A 64-bit integer with the top 48 bits cleared")
    {
        uint64_t input = 0x0000FFFFFFFFFFFF; // Binary: 0000000000000000111111111111111111111111111111111111111111111111

        THEN("The result should be 16")
        {
            REQUIRE(clzll(input) == 16);
        }
    }

    GIVEN("A 64-bit integer with alternating 0s and 1s")
    {
        uint64_t input = 0xAAAAAAAAAAAAAAAA; // Binary: 1010101010101010101010101010101010101010101010101010101010101010

        THEN("The result should be 0")
        {
            REQUIRE(clzll(input) == 0);
        }
    }
}

SCENARIO("compute_minrun function")
{
    GIVEN("An array size from 0 to 63")
    {
        THEN("Minimum run length is equal to size")
        {
            REQUIRE(compute_minrun(0) == 0);
            REQUIRE(compute_minrun(1) == 1);
            REQUIRE(compute_minrun(12) == 12);

            REQUIRE(compute_minrun(32) == 32);
            REQUIRE(compute_minrun(63) == 63);
        }
    }

    GIVEN("An array size greater or equal to 64")
    {
        THEN("Minimum run length is between 32 and 64")
        {
            REQUIRE(compute_minrun(64) == 32);
            REQUIRE(compute_minrun(100) == 50);
            REQUIRE(compute_minrun(1000) == 63);
        }
    }
}

SCENARIO("binary_insertion_find function")
{
    WHEN("Searching in an empty array")
    {
        std::array<int, 0> arr;

        THEN("The position should be 0")
        {
            REQUIRE(binary_insertion_find(arr.data(), 5, arr.size()) == 0);
        }
    }

    GIVEN("An array with a single element")
    {
        int element = 10;
        int biggerThanElement = 15;
        int smallerThanElement = 5;

        std::array<int, 1> arr = { element };

        WHEN("Searching for an element smaller than the given element")
        {
            auto pos = binary_insertion_find(arr.data(), smallerThanElement, arr.size());

            THEN("The position should be 0")
            {
                REQUIRE(pos == 0);
            }
        }

        WHEN("Searching for an element equal to the given element")
        {
            size_t pos = binary_insertion_find(arr.data(), element, arr.size());

            THEN("The position should be 1")
            {
                REQUIRE(pos == 1);
            }
        }

        WHEN("Searching for an element larger than the single element")
        {
            size_t pos = binary_insertion_find(arr.data(), biggerThanElement, arr.size());

            THEN("The position should be 1")
            {
                REQUIRE(pos == 1);
            }
        }
    }

    GIVEN("An array with multiple elements")
    {
        std::array<int, 5> arr = { 10, 20, 30, 40, 50 };

        WHEN("Searching for an element smaller than the smallest element")
        {
            size_t pos = binary_insertion_find(arr.data(), 5, arr.size());

            THEN("The position should be 0")
            {
                REQUIRE(pos == 0);
            }
        }

        WHEN("Searching for an element larger than the largest element")
        {
            size_t pos = binary_insertion_find(arr.data(), 60, arr.size());

            THEN("The position should be 5 (end of the array)")
            {
                REQUIRE(pos == 5);
            }
        }

        WHEN("Searching for an element in the middle of the array")
        {
            size_t pos = binary_insertion_find(arr.data(), 25, arr.size());

            THEN("The position should be 2")
            {
                REQUIRE(pos == 2);
            }
        }

        WHEN("Searching for an element equal to the existing element")
        {
            size_t pos = binary_insertion_find(arr.data(), 30, arr.size());

            THEN("The position should be 3")
            {
                REQUIRE(pos == 3);
            }
        }
    }

    GIVEN("An array with all identical elements")
    {
        std::array<int, 5> arr = { 10, 10, 10, 10, 10 };

        WHEN("Searching for an element equal to the identical elements")
        {
            size_t pos = binary_insertion_find(arr.data(), 10, arr.size());

            THEN("The position should be at the end of the array")
            {
                REQUIRE(pos == arr.size());
            }
        }

        WHEN("Searching for an element smaller than the identical elements")
        {
            size_t pos = binary_insertion_find(arr.data(), 5, arr.size());

            THEN("The position should be 0")
            {
                REQUIRE(pos == 0);
            }
        }

        WHEN("Searching for an element larger than the identical elements")
        {
            size_t pos = binary_insertion_find(arr.data(), 15, arr.size());

            THEN("The position should be at the end of the array")
            {
                REQUIRE(pos == arr.size());
            }
        }
    }
}

SCENARIO("binary_insertion_sort_start function")
{
    GIVEN("An already sorted array")
    {
        std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        std::array<int, 5> expected = { 1, 2, 3, 4, 5 };

        WHEN("Calling with start = 1")
        {
            binary_insertion_sort_start(arr.data(), 1, arr.size());

            THEN("The array remains sorted")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("A reverse sorted array")
    {
        std::array<int, 5> arr = { 5, 4, 3, 2, 1 };
        std::array<int, 5> expected = { 1, 2, 3, 4, 5 };

        WHEN("Calling with start = 3")
        {
            binary_insertion_sort_start(arr.data(), 1, arr.size());

            THEN("The array becomes sorted")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with negative and positive numbers")
    {
        std::array<int, 5> arr = { -3, 2, -1, 4, 0 };
        std::array<int, 5> expected = { -3, 2, -1, 4, 0 };

        WHEN("Calling with start equal to the size of the array")
        {
            binary_insertion_sort_start(arr.data(), arr.size(), arr.size());

            THEN("The array doesn't become sorted")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array of size one")
    {
        std::array<int, 1> arr = { 1 };

        WHEN("Calling with start = 1")
        {
            binary_insertion_sort_start(arr.data(), 1, arr.size());

            THEN("The element remains unchanged")
            {
                REQUIRE(arr.at(0) == 1);
            }
        }
    }

    GIVEN("An array of size zero")
    {
        std::array<int, 0> arr;

        WHEN("Calling with start = 1")
        {
            binary_insertion_sort_start(arr.data(), 1, arr.size());

            THEN("Nothing happens and no crash occurs")
            {
            }
        }
    }
}

SCENARIO("reverse_elements function")
{
    GIVEN("An empty array")
    {
        std::array<int, 0> arr;

        reverse_elements(arr.data(), 0, arr.size() - 1);

        THEN("The array remains unchanged")
        {
            REQUIRE(arr.size() == 0);
        }
    }

    GIVEN("A single-element array")
    {
        std::array<int, 1> arr = { 1 };

        WHEN("End is 0")
        {
            reverse_elements(arr.data(), 0, 0);

            THEN("The array remains unchanged")
            {
                REQUIRE(arr.at(0) == 1);
            }
        }
    }

    GIVEN("An array with only positive elements")
    {
        std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        std::array<int, 5> expected = { 5, 4, 3, 2, 1 };

        WHEN("Reversing the entire array")
        {
            reverse_elements(arr.data(), 0, arr.size() - 1);

            THEN("The array is reversed")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with only negative numbers")
    {
        std::array<int, 4> arr = { -10, -20, -30, -40 };
        std::array<int, 4> expected = { -40, -30, -20, -10 };

        WHEN("Reversing the entire array")
        {
            reverse_elements(arr.data(), 0, arr.size() - 1);

            THEN("The array is reversed")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with positive and negative numbers")
    {
        std::array<int, 5> arr = { -5, -3, 0, 2, 4 };
        std::array<int, 5> expected = { -5, 2, 0, -3, 4 };

        WHEN("Reversing form 1s to 3rd element")
        {
            reverse_elements(arr.data(), 1, 3);

            THEN("Only the sub array is reversed")
            {
                REQUIRE(arr == expected);
            }
        }
    }
}

SCENARIO("count_run function")
{
    GIVEN("An empty array")
    {
        std::array<int, 0> arr;
        size_t run_length = count_run(arr.data(), 0, arr.size());

        THEN("The run length is 0")
        {
            REQUIRE(run_length == 0);
        }
    }

    GIVEN("An array with a single element")
    {
        std::array<int, 1> arr = { 123 };

        size_t run_length = count_run(arr.data(), 0, arr.size());

        THEN("The run length is 1")
        {
            REQUIRE(run_length == 1);
        }
    }

    GIVEN("An array with an increasing sequence")
    {
        std::array<int, 5> arr = { 1, 2, 3, 4, 5 };

        WHEN("Start from 0")
        {
            size_t run_length = count_run(arr.data(), 0, arr.size());

            THEN("The run length is the size of the array")
            {
                REQUIRE(run_length == arr.size());
            }
        }
    }

    GIVEN("An array with a decreasing sequence")
    {
        std::array<int, 5> arr = { 5, 4, 3, 2, 1 };
        std::array<int, 5> expected = { 1, 2, 3, 4, 5 };

        WHEN("Start from 0")
        {
            size_t run_length = count_run(arr.data(), 0, arr.size());

            THEN("The run length is the size of the array and the array is reversed")
            {
                REQUIRE(run_length == arr.size());
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with a mixed sequence")
    {
        std::array<int, 5> arr = { 1, 2, 3, 2, 1 };

        WHEN("Start from 0")
        {
            size_t run_length = count_run(arr.data(), 0, arr.size());

            THEN("The run length is the length of the first increasing run")
            {
                REQUIRE(run_length == 3);
            }
        }
    }

    GIVEN("An array with identical elements")
    {
        std::array<int, 4> arr = { 42, 42, 42, 42 };

        size_t run_length = count_run(arr.data(), 0, arr.size());

        THEN("The run length is the size of the array")
        {
            REQUIRE(run_length == arr.size());
        }
    }

    GIVEN("An array with equal consecutive elements")
    {
        std::array<int, 5> arr = { 1, 1, 1, 3, 5 };
        size_t run_length = count_run(arr.data(), 0, arr.size());

        THEN("The run length is the size of the array")
        {
            REQUIRE(run_length == 5);
        }
    }

    GIVEN("An array with elements")
    {
        std::array<int, 4> arr = { 1, 2, 3, 42 };

        WHEN("Start from the last element")
        {
            size_t run_length = count_run(arr.data(), 3, arr.size());

            THEN("The run length is 1")
            {
                REQUIRE(run_length == 1);
            }
        }
    }
}

SCENARIO("check_invariant function")
{
    GIVEN("An empty stack")
    {
        TIM_SORT_RUN_T stack[1];
        int stack_curr = 0;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant holds")
        {
            REQUIRE(result == 1);
        }
    }

    GIVEN("A stack with a single run")
    {
        TIM_SORT_RUN_T stack[1];
        stack[0] = { 0, 10 };
        int stack_curr = 1;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant holds")
        {
            REQUIRE(result == 1);
        }
    }

    GIVEN("Two runs where 1st > 2nd")
    {
        TIM_SORT_RUN_T stack[2];
        stack[0] = { 0, 10 };   // 1st
        stack[1] = { 10, 5 };   // 2nd
        int stack_curr = 2;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant holds")
        {
            REQUIRE(result == 1);
        }
    }

    GIVEN("Two runs where 1st <= 2nd")
    {
        TIM_SORT_RUN_T stack[2];
        stack[0] = { 0, 5 };    // 1st
        stack[1] = { 5, 10 };   // 2nd
        int stack_curr = 2;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant violated")
        {
            REQUIRE(result == 0);
        }
    }

    GIVEN("Three runs where 1st > 2nd + 3rd and 2nd > 3rd")
    {
        TIM_SORT_RUN_T stack[3];
        stack[0] = { 0, 20 };   // 1st
        stack[1] = { 20, 10 };  // 2nd
        stack[2] = { 30, 5 };   // 3rd
        int stack_curr = 3;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant holds")
        {
            REQUIRE(result == 1);
        }
    }

    GIVEN("Three runs where 1st <= 2nd + 3rd and 2nd <= 3rd")
    {
        TIM_SORT_RUN_T stack[3];
        stack[0] = { 0, 10 };   // 1st
        stack[1] = { 10, 15 };  // 2nd
        stack[2] = { 25, 20 };  // 3rd
        int stack_curr = 3;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant violated")
        {
            REQUIRE(result == 0);
        }
    }

    GIVEN("Three runs where 1st <= 2nd + 3rd and 2nd > 3rd")
    {
        TIM_SORT_RUN_T stack[3];
        stack[0] = { 0, 15 };   // 1st
        stack[1] = { 15, 10 };  // 2nd
        stack[2] = { 25, 6 };   // 3rd
        int stack_curr = 3;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant violated")
        {
            REQUIRE(result == 0);
        }
    }

    GIVEN("Three runs where 1st > 2nd + 3rd and 2nd <= 3rd")
    {
        TIM_SORT_RUN_T stack[3];
        stack[0] = { 0, 40 };   // 1st
        stack[1] = { 40, 5 };   // 2nd
        stack[2] = { 45, 10 };  // 3rd
        int stack_curr = 3;

        int result = check_invariant(stack, stack_curr);

        THEN("Invariant violated")
        {
            REQUIRE(result == 0);
        }
    }
}

SCENARIO("tim_sort_resize function")
{
    GIVEN("A TEMP_STORAGE_T with allocated storage")
    {
        TEMP_STORAGE_T<int> store;
        store.alloc = 10;
        store.storage = reinterpret_cast<int*>(timsort::detail::malloc(store.alloc * sizeof(int)));

        WHEN("tim_sort_resize is called with a smaller new_size")
        {
            tim_sort_resize(&store, 5);

            THEN("The allocation size remains unchanged")
            {
                REQUIRE(store.alloc == 10);
                REQUIRE(store.storage != nullptr);
            }
        }

        WHEN("tim_sort_resize is called with a larger new_size")
        {
            tim_sort_resize(&store, 15);

            THEN("The allocation size is updated and storage is resized")
            {
                REQUIRE(store.alloc == 15);
                REQUIRE(store.storage != nullptr);
            }
        }

        WHEN("tim_sort_resize is called with exactly the same size")
        {
            tim_sort_resize(&store, 15);

            THEN("Nothing changes")
            {
                REQUIRE(store.alloc == 15);
                REQUIRE(store.storage != nullptr);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("A TEMP_STORAGE_T object with null storage")
    {
        TEMP_STORAGE_T<int> store;
        store.alloc = 0;
        store.storage = nullptr;

        WHEN("tim_sort_resize is called with a non-zero new_size")
        {
            tim_sort_resize(&store, 10);

            THEN("The storage is allocated and allocation size is updated")
            {
                REQUIRE(store.alloc == 10);
                REQUIRE(store.storage != nullptr);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("An invalid TEMP_STORAGE_T with alloc > 0 but null storage")
    {
        TEMP_STORAGE_T<int> store;
        store.alloc = 5;
        store.storage = nullptr;

        WHEN("tim_sort_resize is called with new_size > alloc")
        {
            tim_sort_resize(&store, 10);

            THEN("New allocation is made safely")
            {
                REQUIRE(store.alloc == 10);
                REQUIRE(store.storage != nullptr);
            }
        }

        timsort::detail::free(store.storage);
    }
}

SCENARIO("tim_sort_merge function")
{
    GIVEN("Two sorted runs of equal size")
    {
        std::array<int, 6> arr = { 1, 2, 3, 4, 5, 6 };
        std::array<int, 6> expected = { 1, 2, 3, 4, 5, 6 };
        TEMP_STORAGE_T<int> store = {};

        TIM_SORT_RUN_T stack[] = {
            {0, 3}, // [1, 3, 5]
            {3, 3}  // [2, 4, 6]
        };

        WHEN("Merging the runs")
        {
            tim_sort_merge(arr.data(), stack, 2, &store);

            THEN("The array becomes sorted")
            {
                REQUIRE(arr == expected);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Two runs where one is empty")
    {
        std::array<int, 3> arr = { 1, 2, 3 };
        std::array<int, 3> expected = { 1, 2, 3 };
        TEMP_STORAGE_T<int> store = {};

        TIM_SORT_RUN_T stack[] = {
            {0, 3}, // [1, 2, 3]
            {3, 0}  // []
        };

        WHEN("Merging the runs")
        {
            tim_sort_merge(arr.data(), stack, 2, &store);

            THEN("The array remains unchanged")
            {
                REQUIRE(arr == expected);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Both runs are empty")
    {
        std::array<int, 0> arr = {};
        TEMP_STORAGE_T<int> store = {};
        TIM_SORT_RUN_T stack[] = {
            {0, 0},
            {0, 0}
        };

        WHEN("Merging the empty runs")
        {
            tim_sort_merge(arr.data(), stack, 2, &store);

            THEN("Nothing happens and no crash occurs")
            {
                REQUIRE(arr.size() == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Two runs with duplicate values")
    {
        std::array<int, 6> arr = { 1, 3, 5, 3, 4, 6 };
        std::array<int, 6> expected = { 1, 3, 3, 4, 5, 6 };
        TEMP_STORAGE_T<int> store = {};

        TIM_SORT_RUN_T stack[] = {
            {0, 3}, // [1, 3, 5]
            {3, 3}  // [3, 4, 6]
        };

        WHEN("Merging runs with duplicates")
        {
            tim_sort_merge(arr.data(), stack, 2, &store);

            THEN("The result is a sorted array")
            {
                REQUIRE(arr == expected);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Each run has one element")
    {
        std::array<int, 2> arr = { 5, 2 };
        std::array<int, 2> expected = { 2, 5 };
        TEMP_STORAGE_T<int> store = {};

        TIM_SORT_RUN_T stack[] = {
            {0, 1}, // [5]
            {1, 1}  // [2]
        };

        WHEN("Merging two one-element runs")
        {
            tim_sort_merge(arr.data(), stack, 2, &store);

            THEN("They are sorted")
            {
                REQUIRE(arr == expected);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("All values are equal")
    {
        std::array<int, 6> arr = { 7, 7, 7, 7, 7, 7 };
        std::array<int, 6> expected = { 7, 7, 7, 7, 7, 7 };
        TEMP_STORAGE_T<int> store = {};

        TIM_SORT_RUN_T stack[] = {
            {0, 3}, // [7, 7, 7]
            {3, 3}  // [7, 7, 7]
        };

        WHEN("Merging equal values")
        {
            tim_sort_merge(arr.data(), stack, 2, &store);

            THEN("They are merged")
            {
                REQUIRE(arr == expected);
            }
        }

        timsort::detail::free(store.storage);
    }
}

SCENARIO("tim_sort_collapse function")
{
    GIVEN("Stack with only one run")
    {
        std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        TIM_SORT_RUN_T expectedStack[] = { {0, 5} };
        TEMP_STORAGE_T<int> store = {};
        int stack_curr = 1;
        TIM_SORT_RUN_T stack[] = { {0, 5} };

        WHEN("Collapsing a single-run stack")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_curr, &store, arr.size());

            THEN("Stack remains unchanged")
            {
                REQUIRE(result == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Stack with two runs that sum to full array size")
    {
        std::array<int, 4> arr = { 1, 2, 3, 4 };
        TEMP_STORAGE_T<int> store = {};
        int stack_size = 2;
        TIM_SORT_RUN_T stack[] = {
            {0, 2}, // [1,3]
            {2, 2}  // [2,4]
        };

        TIM_SORT_RUN_T expectedStack[] = {
            {0, 4},
            {0, 0}
        };

        WHEN("Collapsing a two-run stack that covers full array")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_size, &store, arr.size());

            THEN("Runs are merged into one run")
            {
                REQUIRE(result == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Stack with two runs violating invariant (stack[0].length <= stack[1].length)")
    {
        std::array<int, 4> arr = { 1, 2, 3, 4 };
        TEMP_STORAGE_T<int> store = {};
        int stack_size = 2;
        TIM_SORT_RUN_T stack[] = {
            {0, 2},
            {2, 2}
        };

        TIM_SORT_RUN_T expectedStack[] = {
            {0, 4},
            {0, 0}
        };

        WHEN("Collapsing stack where first run length <= second run length")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_size, &store, arr.size());

            THEN("Runs are merged into one run")
            {
                REQUIRE(result == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Stack with three runs triggering left merge")
    {
        std::array<int, 13> arr { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};

        // To avoid full collapse after the left merge (which leaves only 2 runs),
        // we need to start with more than 3 runs.

        // B <= C + D - left merge
        // C > D
        // A > B + C
        TIM_SORT_RUN_T stack[] = {
            {0, 7},  // A
            {7, 3},  // B
            {10, 2}, // C
            {12, 1}  // D
        };
        int stack_size = 4;
        TEMP_STORAGE_T<int> store = {};
        TIM_SORT_RUN_T expectedStack[] = {
            {0, 7},
            {7, 5},
            {12, 1},
            {0, 0}  // Last run is empty after merge
        };

        WHEN("Collapsing stack with left merge condition")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_size, &store, arr.size());

            THEN("Left merge occurs and stack size decreases")
            {
                REQUIRE(result == 3);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Stack with three runs triggering right merge")
    {
        std::array<int, 15> arr{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

        // To avoid full collapse after the right merge (which leaves only 2 runs),
        // we need to start with more than 3 runs.

        // C <= D - right merge
        // B > C + D
        // A > B + C
        TIM_SORT_RUN_T stack[] = {
            {0, 8},  // A
            {8, 4},  // B
            {12, 1}, // C
            {13, 2}  // D
        };
        int stack_size = 4;
        TEMP_STORAGE_T<int> store = {};
        TIM_SORT_RUN_T expectedStack[] = {
            {0, 8},
            {8, 4},
            {12, 3},
            {0, 0}  // Last run is empty after merge
        };

        WHEN("Collapsing stack with right merge condition")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_size, &store, arr.size());

            THEN("Right merge occurs and stack size decreases")
            {
                REQUIRE(result == 3);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Stack with 4 runs where invariants hold")
    {
        std::array<int, 14> arr{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14 };

        // B > C + D
        // C > D
        // A > B + C
        TIM_SORT_RUN_T stack[] = {
            {0, 7},  //A
            {7, 4},  //B
            {11, 2}, //C
            {13, 1}  //D
        };
        int stack_size = 4;
        TEMP_STORAGE_T<int> store = {};
        TIM_SORT_RUN_T expectedStack[] = {
            {0, 7},
            {7, 4},
            {11, 2},
            {13, 1}
        };

        WHEN("Collapsing stack with no merges needed")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_size, &store, arr.size());

            THEN("Stack remains unchanged and data intact")
            {
                REQUIRE(result == 4);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Stack_curr == 0)")
    {
        std::array<int, 0> arr = {};
        TIM_SORT_RUN_T stack[1] = {};
        int stack_size = 0;
        TEMP_STORAGE_T<int> store = {};

        WHEN("Collapsing empty stack")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_size, &store, arr.size());

            THEN("Returns zero and does nothing")
            {
                REQUIRE(result == 0);
            }
        }
    }

    GIVEN("Stack with runs smaller than the size parameter")
    {
        std::array<int, 4> arr { 4, 3, 2, 1 };

        TIM_SORT_RUN_T stack[] = {
            {0, 2},
            {2, 1}
        };
        int stack_size = 2;
        TEMP_STORAGE_T<int> store = {};
        TIM_SORT_RUN_T expectedStack[] = {
            {0, 2},
            {2, 1}
        };

        WHEN("Collapsing stack with size larger than runs sum")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_size, &store, 4);

            THEN("Function does not merge")
            {
                REQUIRE(result == 2);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }

    GIVEN("Stack with four runs triggering ABC condition merge")
    {
        std::array<int, 8> arr { 1, 2, 3, 4, 5, 6, 7, 8 };

        // A <= B + C - condition merge
        // B > C + D
        // C > D
        TIM_SORT_RUN_T stack[] = {
            {0, 1},  // A
            {1, 4},  // B
            {5, 2},  // C
            {7, 1}   // D
        };
        int stack_curr = 4;
        TEMP_STORAGE_T<int> store = {};
        TIM_SORT_RUN_T expectedStack[] = {
            {0, 8},
            {0, 0},
            {0, 0},
            {0, 0}
        };

        WHEN("Collapsing stack with ABC condition")
        {
            int result = tim_sort_collapse(arr.data(), stack, stack_curr, &store, arr.size());

            THEN("Left merge of B and C occurs")
            {
                REQUIRE(result == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
            }
        }

        timsort::detail::free(store.storage);
    }
}

SCENARIO("PUSH_NEXT function")
{
    GIVEN("Large unsorted array of integers")
    {
        constexpr uint64_t arrSize = 64; // Minimum array size to get minrun less than size of array
        int unsortedArray[arrSize] = {
            42, 7, 23, 89, 1, 56, 33, 12,
            77, 5, 38, 91, 44, 18, 29, 65,
            3, 50, 11, 27, 62, 9, 74, 36,
            20, 53, 8, 41, 14, 67, 2, 81,
            24, 6, 55, 30, 13, 70, 4, 59,
            17, 39, 10, 66, 26, 78, 15, 31,
            61, 19, 47, 25, 35, 49, 16, 73,
            28, 22, 68, 32, 40, 21, 60, 34
        };

        TEMP_STORAGE_T<int> store;
        TIM_SORT_RUN_T stack[kStackSize];
        size_t stack_curr = 0;
        size_t curr = 0;
        size_t minrun = compute_minrun(arrSize);

        TIM_SORT_RUN_T expectedStack[] = { {0, 32} };

        WHEN("PUSH_NEXT is called on the unsorted array")
        {
            int result = PUSH_NEXT(unsortedArray, arrSize, &store, minrun, stack, &stack_curr, &curr);

            THEN("The function should correctly identify the first run")
            {
                REQUIRE(result == 1);
                REQUIRE(stack_curr == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
                REQUIRE(curr == 32);
            }
        }
    }

    GIVEN("Not too big unsorted array of integers")
    {
        std::array<int, 5> arr{ 1, 2, 3, 4, 5};
        TEMP_STORAGE_T<int> store;
        TIM_SORT_RUN_T stack[kStackSize];
        size_t stack_curr = 0;
        size_t curr = 0;
        size_t minrun = compute_minrun(arr.size());

        TIM_SORT_RUN_T expectedStack[] = { {0, 5} };

        WHEN("PUSH_NEXT is called on the unsorted array")
        {
            int result = PUSH_NEXT(arr.data(), arr.size(), &store, minrun, stack, &stack_curr, &curr);

            THEN("The function should sort the entire array as there is no reason to divide it into parts")
            {
                REQUIRE(result == 0);
                REQUIRE(stack_curr == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
                REQUIRE(curr == 5);
            }
        }
    }

    GIVEN("A sorted array of integers")
    {
        std::array<int, 10> arr{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
        TEMP_STORAGE_T<int> store;
        TIM_SORT_RUN_T stack[kStackSize];
        size_t stack_curr = 0;
        size_t curr = 0;
        size_t minrun = compute_minrun(arr.size());

        TIM_SORT_RUN_T expectedStack[] = { {0, arr.size()} };

        WHEN("PUSH_NEXT is called on the sorted array")
        {
            int result = PUSH_NEXT(arr.data(), arr.size(), &store, minrun, stack, &stack_curr, &curr);

            THEN("The function should identify the entire array as a single run")
            {
                REQUIRE(result == 0);
                REQUIRE(stack_curr == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
                REQUIRE(curr == arr.size());
            }
        }
    }

    GIVEN("An empty array")
    {
        std::array<int, 0> arr = {};
        TEMP_STORAGE_T<int> store;
        TIM_SORT_RUN_T stack[kStackSize];
        size_t stack_curr = 0;
        size_t curr = 0;
        size_t minrun = compute_minrun(arr.size());

        WHEN("PUSH_NEXT is called on the empty array")
        {
            int result = PUSH_NEXT(arr.data(), arr.size(), &store, minrun, stack, &stack_curr, &curr);

            THEN("The function should handle the empty array")
            {
                REQUIRE(result == 0);
                REQUIRE(stack_curr == 0);
                REQUIRE(curr == 0);
            }
        }
    }

    GIVEN("An array with one element")
    {
        std::array<int, 1> arr = { 1 };
        TEMP_STORAGE_T<int> store;
        TIM_SORT_RUN_T stack[kStackSize];
        size_t stack_curr = 0;
        size_t curr = 0;
        size_t minrun = compute_minrun(arr.size());

        TIM_SORT_RUN_T expectedStack[] = { {0, 1} };

        WHEN("PUSH_NEXT is called on the array with one element")
        {
            int result = PUSH_NEXT(arr.data(), arr.size(), &store, minrun, stack, &stack_curr, &curr);

            THEN("The function should identify the single element as a run")
            {
                REQUIRE(result == 0);
                REQUIRE(stack_curr == 1);
                REQUIRE(memcmp(stack, expectedStack, sizeof(expectedStack)) == 0);
                REQUIRE(curr == 1);
            }
        }

        if (store.storage)
        {
            timsort::detail::free(store.storage);
        }
    }
}

SCENARIO("binary_insertion_sort function")
{
    GIVEN("An unsorted array of integers")
    {
        std::array<int, 5> arr{ 5, 2, 9, 1, 6 };
        std::array<int, 5> expected = { 1, 2, 5, 6, 9 };

        WHEN("Calling binary_insertion_sort")
        {
            timsort::binary_insertion_sort(arr.data(), arr.size());

            THEN("The array becomes sorted")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An empty array")
    {
        std::array<int, 0> arr;

        WHEN("Calling binary_insertion_sort")
        {
            timsort::binary_insertion_sort(arr.data(), arr.size());

            THEN("Nothing happens and no crash occurs")
            {
                REQUIRE(arr.size() == 0);
            }
        }
    }

    GIVEN("An array with a single element")
    {
        std::array<int, 1> arr = { 1 };

        WHEN("Calling binary_insertion_sort")
        {
            timsort::binary_insertion_sort(arr.data(), arr.size());

            THEN("The array remains unchanged")
            {
                REQUIRE(arr.at(0) == 1);
            }
        }
    }

    GIVEN("An array with all elements equal")
    {
        std::array<int, 5> arr = { 7, 7, 7, 7, 7 };
        std::array<int, 5> expected = { 7, 7, 7, 7, 7 };
        
        WHEN("Calling binary_insertion_sort")
        {
            timsort::binary_insertion_sort(arr.data(), arr.size());

            THEN("The array remains unchanged")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with already sorted elements")
    {
        std::array<int, 5> arr = { 1, 2, 3, 4, 5 };
        std::array<int, 5> expected = { 1, 2, 3, 4, 5 };

        WHEN("Calling binary_insertion_sort")
        {
            timsort::binary_insertion_sort(arr.data(), arr.size());

            THEN("The array remains unchanged")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with negative and positive integers")
    {
        std::array<int, 5> arr = { 0, 2, -3, 4, -1 };
        std::array<int, 5> expected = { -3, -1, 0, 2, 4 };

        WHEN("Calling binary_insertion_sort")
        {
            timsort::binary_insertion_sort(arr.data(), arr.size());

            THEN("The array becomes sorted")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with duplicate elements")
    {
        std::array<int, 6> arr = { 3, 1, 2, 3, 4, 3 };
        std::array<int, 6> expected = { 1, 2, 3, 3, 3, 4 };

        WHEN("Calling binary_insertion_sort")
        {
            timsort::binary_insertion_sort(arr.data(), arr.size());

            THEN("The array becomes sorted with duplicates in order")
            {
                REQUIRE(arr == expected);
            }
        }
    }
}

SCENARIO("tim_sort function")
{
    GIVEN("An empty array")
    {
        std::array<int, 0> arr;

        WHEN("Calling tim_sort")
        {
            timsort::tim_sort(arr.data(), arr.size());

            THEN("Nothing happens and no crash occurs")
            {
                REQUIRE(arr.size() == 0);
            }
        }
    }

    GIVEN("An array with a single element")
    {
        std::array<int, 1> arr = { 123 };

        WHEN("Calling tim_sort")
        {
            timsort::tim_sort(arr.data(), arr.size());

            THEN("The array remains unchanged")
            {
                REQUIRE(arr.at(0) == 123);
            }
        }
    }

    GIVEN("An array with <64 elements")
    {
        std::array<int, 5> arr = { 5, 2, 9, 1, 6 };
        std::array<int, 5> expected = { 1, 2, 5, 6, 9 };

        WHEN("Calling tim_sort")
        {
            timsort::tim_sort(arr.data(), arr.size());

            THEN("The array becomes sorted")
            {
                REQUIRE(arr == expected);
            }
        }
    }

    GIVEN("An array with more than 64 elements")
    {
        constexpr int size = 1000;
        std::array<int, size> arr;
        for (int i = size; i > 0; --i)
        {
            arr.at(i - 1) = i;
        }

        std::array<int, size> expected;
        for (int i = 0; i < 1000; ++i)
        {
            expected.at(i) = i + 1;
        }

        WHEN("Calling tim_sort")
        {
            timsort::tim_sort(arr.data(), arr.size());

            THEN("The array becomes sorted")
            {
                REQUIRE(arr == expected);
            }
        }
    }
}