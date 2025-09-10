#include "pch.h"
#include <kf/SpanUtils.h>

SCENARIO("SpanUtils")
{
    GIVEN("an array of int for span_cast from int to std::byte")
    {
        constexpr int kArr[] = { 1, 2, 3, 4 };

        WHEN("span_cast is called on not empty span")
        {
            std::span<const int> intSpan(kArr);
            auto byteSpan = kf::span_cast<const std::byte>(intSpan);

            THEN("the size matches the byte size of the array")
            {
                REQUIRE(byteSpan.size() == sizeof(kArr));
            }
        }

        WHEN("span_cast is called on an empty span")
        {
            const std::span<int> emptySpan;
            auto byteSpan = kf::span_cast<const std::byte>(emptySpan);

            THEN("the result span is empty")
            {
                REQUIRE(byteSpan.size() == 0);
            }
        }
    }

    GIVEN("a pointer to int and a size for span_cast from int* to std::byte span")
    {
        constexpr int kArr[] = { 1, 2, 3, 4 };

        WHEN("span_cast is called")
        {
            auto byteSpan = kf::span_cast<const std::byte>(kArr, 4);

            THEN("the size matches the byte size of the array")
            {
                REQUIRE(byteSpan.size() == sizeof(kArr));
            }
        }

        WHEN("span_cast is called with nullptr and size 0")
        {
            constexpr int* nullArr = nullptr;
            auto nullSpan = kf::span_cast<const std::byte>(nullArr, 0);

            THEN("the result span is empty")
            {
                REQUIRE(nullSpan.size() == 0);
            }
        }
    }

    GIVEN("an int array for as_bytes")
    {
        constexpr int kArr[] = { 42, 99 };

        WHEN("as_bytes is called with pointer and size")
        {
            auto s = kf::as_bytes(kArr, sizeof(kArr));

            THEN("the span covers the array memory")
            {
                REQUIRE(s.size() == sizeof(kArr));
                REQUIRE(reinterpret_cast<const void*>(s.data()) == reinterpret_cast<const void*>(kArr));
            }
        }

        WHEN("as_bytes is called with nullptr and size 0")
        {
            auto nullSpan = kf::as_bytes(nullptr, 0);

            THEN("the result span is empty")
            {
                REQUIRE(nullSpan.size() == 0);
            }
        }
    }

    GIVEN("an int array for as_bytes(T[N])")
    {
        constexpr int kArr[] = { 1, 2, 3 };

        WHEN("as_bytes is called")
        {
            auto s = kf::as_bytes(kArr);

            THEN("the span covers the array memory")
            {
                REQUIRE(s.size() == sizeof(kArr));
                REQUIRE(reinterpret_cast<const void*>(s.data()) == reinterpret_cast<const void*>(kArr));
            }
        }
    }

    GIVEN("an int array for as_writable_bytes")
    {
        int arr[] = { 0, 0 };

        WHEN("as_writable_bytes is called with pointer and size")
        {
            auto s = kf::as_writable_bytes(arr, sizeof(arr));

            THEN("the span covers the array memory and is writable")
            {
                REQUIRE(s.size() == sizeof(arr));
                REQUIRE(reinterpret_cast<const void*>(s.data()) == reinterpret_cast<const void*>(arr));
            }

            THEN("the span is writable")
            {
                s[0] = std::byte{ 0xFF };
                REQUIRE(static_cast<unsigned char>(s[0]) == 0xFF);
            }
        }

        WHEN("as_writable_bytes is called with nullptr and size 0")
        {
            auto nullSpan = kf::as_writable_bytes(nullptr, 0);

            THEN("the result span is empty")
            {
                REQUIRE(nullSpan.size() == 0);
            }
        }
    }

    GIVEN("an int array for as_writable_bytes(T[N])")
    {
        int arr[] = { 0, 0 };

        WHEN("as_writable_bytes is called")
        {
            auto s = kf::as_writable_bytes(arr);

            THEN("the span covers the array memory and is writable")
            {
                REQUIRE(s.size() == sizeof(arr));
                REQUIRE(reinterpret_cast<const void*>(s.data()) == reinterpret_cast<const void*>(arr));
            }

            THEN("the span is writable")
            {
                s[0] = std::byte{ 0xAA };
                REQUIRE(static_cast<unsigned char>(s[0]) == 0xAA);
            }
        }
    }

    GIVEN("source and destination arrays for copyTruncate")
    {
        int arr1[] = { 1, 2, 3, 4, 5 };
        int arr2[] = { 11, 12, 13 };

        WHEN("copyTruncate is called with source larger than destination")
        {
            auto result = kf::copyTruncate(std::span<int>(arr2), std::span<const int>(arr1));

            THEN("only the first destination.size() elements are copied")
            {
                REQUIRE(result.size() == 3);
                constexpr int kExpected[] = { 1, 2, 3 };
                REQUIRE(memcmp(result.data(), kExpected, sizeof(kExpected)) == 0);
            }
        }

        WHEN("copyTruncate is called with src smaller than dst")
        {
            auto result = kf::copyTruncate(std::span<int>(arr1), std::span<const int>(arr2));

            THEN("all src elements are copied")
            {
                REQUIRE(result.size() == 3);
                constexpr int kExpected[] = { 11, 12, 13, 4, 5 };
                REQUIRE(memcmp(result.data(), kExpected, sizeof(kExpected)) == 0);
            }
        }
    }

    GIVEN("source and destination arrays for copyExact")
    {
        WHEN("copyExact is called with equal sizes")
        {
            constexpr int kSrc[] = { 1, 2, 3 };
            int dst[] = { 11, 22, 33 };

            auto result = kf::copyExact(std::span<int>(dst), std::span<const int>(kSrc));

            THEN("all elements are copied")
            {
                REQUIRE(result.size() == 3);
                constexpr int kExpected[] = { 1, 2, 3 };
                REQUIRE(memcmp(result.data(), kExpected, sizeof(kExpected)) == 0);
            }
        }

        WHEN("copyExact is called with mismatched sizes")
        {
            constexpr int kSrc[] = { 1, 2, 3 };
            int dst[] = { 11, 22 };

            THEN("_Xinvalid_argument is called")
            {
                std::g_Xinvalid_argument_call_count = 0;
                kf::copyExact(std::span<int>(dst), std::span<const int>(kSrc));
                REQUIRE(std::g_Xinvalid_argument_call_count == 1);
            }
        }
    }

    GIVEN("source and destination arrays for copy")
    {
        int arr1[] = { 5, 6 };
        int arr2[] = { 55, 66, 77, 88 };

        WHEN("copy is called with source smaller than destination")
        {
            auto result = kf::copy(std::span<int>(arr2), std::span<const int>(arr1));

            THEN("all source elements are copied")
            {
                REQUIRE(result.size() == 2);
                constexpr int kExpected[] = { 5, 6 };
                REQUIRE(memcmp(result.data(), kExpected, sizeof(kExpected)) == 0);
            }
        }

        WHEN("copy is called with source larger than destination")
        {
            THEN("_Xinvalid_argument is called")
            {
                std::g_Xinvalid_argument_call_count = 0;
                kf::copy(std::span<int>(const_cast<int*>(arr1), 2), std::span<const int>(arr2, 4));
                REQUIRE(std::g_Xinvalid_argument_call_count == 1);
            }
        }
    }

    GIVEN("two arrays for equals")
    {
        constexpr int kA[] = { 1, 2, 3 };
        constexpr int kB[] = { 1, 2, 3 };
        constexpr int kC[] = { 1, 2, 4 };

        WHEN("equals is called on identical arrays")
        {
            THEN("returns true")
            {
                REQUIRE(kf::equals(std::span<const int>(kA), std::span<const int>(kB)));
            }
        }

        WHEN("equals is called on different arrays")
        {
            THEN("returns false")
            {
                REQUIRE(!kf::equals(std::span<const int>(kA), std::span<const int>(kC)));
            }
        }

        WHEN("equals is called on empty arrays")
        {
            constexpr int* kLeft = nullptr;
            constexpr int* kRight = nullptr;
            THEN("returns true")
            {
                REQUIRE(kf::equals(std::span<const int>(kLeft, 0), std::span<const int>(kRight, 0)));
            }
        }
    }

    GIVEN("an array for indexOf")
    {
        constexpr int kArr[] = { 10, 20, 30, 40, 50 };

        WHEN("indexOf is called for a present value")
        {
            THEN("returns the correct index")
            {
                REQUIRE(kf::indexOf(std::span<const int>(kArr), 30) == 2);
            }
        }

        WHEN("indexOf is called for a missing value")
        {
            THEN("returns -1")
            {
                REQUIRE(kf::indexOf(std::span<const int>(kArr), 75) == -1);
            }
        }

        WHEN("indexOf is called with fromIndex out of bounds")
        {
            THEN("returns -1")
            {
                REQUIRE(kf::indexOf(std::span<const int>(kArr), 10, 5) == -1);
            }
        }
    }

    GIVEN("an array for split")
    {
        constexpr int kArr[] = { 1, 2, 3, 7, 2, 5, 9, 8 };
        constexpr int kSeparator = 2;
        constexpr int kMissingSeparator = 75;

        WHEN("split is called with idx before first separator")
        {
            ptrdiff_t idx = 0;
            auto s = kf::split(std::span<const int>(kArr), kSeparator, idx);

            THEN("returns the first segment before separator")
            {
                REQUIRE(s.size() == 1);
                REQUIRE(s[0] == 1);
            }

            THEN("idx is updated to the first separator index")
            {
                REQUIRE(idx == 2);
            }
        }

        WHEN("split is called with idx between separators")
        {
            ptrdiff_t idx = 2;
            auto s = kf::split(std::span<const int>(kArr), kSeparator, idx);

            THEN("returns the segment after first separator before last separator")
            {
                REQUIRE(s.size() == 2);
                REQUIRE(s[0] == 3);
                REQUIRE(s[1] == 7);
            }

            THEN("idx is updated to the next separator index")
            {
                REQUIRE(idx == 5);
            }
        }

        WHEN("split is called with idx after the last separator")
        {
            ptrdiff_t idx = 5;
            auto s = kf::split(std::span<const int>(kArr), kSeparator, idx);

            THEN("returns the last segment after separator")
            {
                REQUIRE(s.size() == 3);
                REQUIRE(s[0] == 5);
                REQUIRE(s[1] == 9);
                REQUIRE(s[2] == 8);
            }

            THEN("idx is updated to -1 indicating no more separators")
            {
                REQUIRE(idx == -1);
            }
        }

        WHEN("split is called with no separator found")
        {
            ptrdiff_t idx = 0;
            auto s = kf::split(std::span<const int>(kArr), kMissingSeparator, idx);

            THEN("returns the whole array")
            {
                REQUIRE(s.size() == 8);
                REQUIRE(reinterpret_cast<const void*>(s.data()) == reinterpret_cast<const void*>(kArr));
            }

            THEN("idx is updated to -1 indicating no more separators")
            {
                REQUIRE(idx == -1);
            }
        }
    }

    GIVEN("an array for atOrDefault")
    {
        constexpr int kArr[3] = { 7, 8, 9 };
        constexpr int kDefault = -1;
        constexpr int kOtherDefault = 42;

        WHEN("atOrDefault is called with a valid index")
        {
            THEN("returns the element at that index")
            {
                REQUIRE(kf::atOrDefault(std::span<const int>(kArr), 1, kDefault) == 8);
            }
        }

        WHEN("atOrDefault is called with an invalid index")
        {
            THEN("returns the default value")
            {
                REQUIRE(kf::atOrDefault(std::span<const int>(kArr), 5, kDefault) == kDefault);
            }
        }

        WHEN("atOrDefault is called on an empty span")
        {
            constexpr int* kEmpty = nullptr;
            THEN("returns the default value")
            {
                REQUIRE(kf::atOrDefault(std::span<const int>(kEmpty, 0), 0, kOtherDefault) == kOtherDefault);
            }
        }
    }
}