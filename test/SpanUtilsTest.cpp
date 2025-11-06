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

            THEN("the size_bytes matches the intSpan and byteSpan")
            {
                REQUIRE(byteSpan.size_bytes() == intSpan.size_bytes());
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
            constexpr int* kNullArr = nullptr;
            auto nullSpan = kf::span_cast<const std::byte>(kNullArr, 0);

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
                REQUIRE(static_cast<const void*>(s.data()) == static_cast<const void*>(kArr));
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
                REQUIRE(static_cast<const void*>(s.data()) == static_cast<const void*>(kArr));
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
                REQUIRE(static_cast<const void*>(s.data()) == static_cast<const void*>(arr));
            }

            THEN("the span is writable")
            {
                s[0] = std::byte{ 0xFF };
                REQUIRE(s[0] == std::byte{ 0xFF });
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
                REQUIRE(static_cast<const void*>(s.data()) == static_cast<const void*>(arr));
            }

            THEN("the span is writable")
            {
                s[0] = std::byte{ 0xAA };
                REQUIRE(s[0] == std::byte{ 0xAA });
            }
        }
    }

    GIVEN("source and destination arrays for copyTruncate")
    {
        int biggerArr[] = { 1, 2, 3, 4, 5 };
        int smallerArr[] = { 11, 12, 13 };

        WHEN("copyTruncate is called with source larger than destination")
        {
            auto result = kf::copyTruncate(std::span{ smallerArr }, std::span{ biggerArr });

            THEN("only the first destination.size() elements are copied")
            {
                constexpr int kExpected[] = { 1, 2, 3 };
                REQUIRE(std::ranges::equal(result, kExpected));
            }
        }

        WHEN("copyTruncate is called with src smaller than dst")
        {
            auto result = kf::copyTruncate(std::span{ biggerArr }, std::span{ smallerArr });

            THEN("all src elements are copied")
            {
                constexpr int kExpectedResult[] = { 11, 12, 13 };
                REQUIRE(std::ranges::equal(result, kExpectedResult));
                
                constexpr int kExpectedBiggerArr[] = { 11, 12, 13, 4, 5 };
                REQUIRE(std::ranges::equal(biggerArr, kExpectedBiggerArr));
            }
        }
    }

    GIVEN("source and destination arrays for copyExact")
    {
        WHEN("copyExact is called with equal sizes")
        {
            constexpr int kSrc[] = { 1, 2, 3 };
            int dst[] = { 11, 22, 33 };

            auto result = kf::copyExact(std::span{ dst }, std::span{ kSrc });

            THEN("all elements are copied")
            {
                constexpr int kExpected[] = { 1, 2, 3 };
                REQUIRE(std::ranges::equal(result, kExpected));
            }
        }
    }

    GIVEN("source and destination arrays for copy")
    {
        int smallerArr[] = { 5, 6 };
        int biggerArr[] = { 55, 66, 77, 88 };

        WHEN("copy is called with source smaller than destination")
        {
            auto result = kf::copy(std::span{ biggerArr }, std::span{ smallerArr });

            THEN("all source elements are copied")
            {
                constexpr int kExpected[] = { 5, 6 };
                REQUIRE(std::ranges::equal(result, kExpected));
            }
        }
    }

    GIVEN("arrays for equals")
    {
        constexpr int kA[] = { 1, 2, 3 };
        constexpr int kB[] = { 1, 2, 3 };
        constexpr int kC[] = { 1, 2, 4 };
        constexpr int kD[] = { 1, 2, 3, 4 };

        WHEN("equals is called on identical arrays")
        {
            THEN("returns true")
            {
                REQUIRE(kf::equals(std::span{ kA }, std::span{ kB }));
            }
        }

        WHEN("equals is called on different array content")
        {
            THEN("returns false")
            {
                REQUIRE(!kf::equals(std::span{ kA }, std::span{ kC }));
            }
        }

        WHEN("equals is called on different array size")
        {
            THEN("returns false")
            {
                REQUIRE(!kf::equals(std::span{ kC }, std::span{ kD }));
            }
        }

        WHEN("equals is called on empty arrays")
        {
            constexpr int* kLeft = nullptr;
            constexpr int* kRight = nullptr;

            THEN("returns true")
            {
                REQUIRE(kf::equals(std::span{ kLeft, 0 }, std::span{ kRight, 0 }));
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
                REQUIRE(kf::indexOf(std::span{ kArr }, 30) == 2);
            }
        }

        WHEN("indexOf is called for a missing value")
        {
            THEN("returns -1")
            {
                REQUIRE(kf::indexOf(std::span{ kArr }, 75) == -1);
            }
        }

        WHEN("indexOf is called with fromIndex out of bounds")
        {
            THEN("returns -1")
            {
                REQUIRE(kf::indexOf(std::span{ kArr }, 10, 5) == -1);
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
            auto result = kf::split(std::span{ kArr }, kSeparator, idx);

            THEN("returns the first segment before separator")
            {
                constexpr int kExpected[] = { 1 };
                REQUIRE(std::ranges::equal(result, kExpected));
            }

            THEN("idx is updated to the first separator index")
            {
                REQUIRE(idx == 2);
            }
        }

        WHEN("split is called with idx between separators")
        {
            ptrdiff_t idx = 2;
            auto result = kf::split(std::span{ kArr }, kSeparator, idx);

            THEN("returns the segment after first separator before last separator")
            {
                constexpr int kExpected[] = { 3, 7 };
                REQUIRE(std::ranges::equal(result, kExpected));
            }

            THEN("idx is updated to the next separator index")
            {
                REQUIRE(idx == 5);
            }
        }

        WHEN("split is called with idx after the last separator")
        {
            ptrdiff_t idx = 5;
            auto result = kf::split(std::span{ kArr }, kSeparator, idx);

            THEN("returns the last segment after separator")
            {
                constexpr int kExpected[] = { 5, 9, 8 };
                REQUIRE(std::ranges::equal(result, kExpected));
            }

            THEN("idx is updated to -1 indicating no more separators")
            {
                REQUIRE(idx == -1);
            }
        }

        WHEN("split is called with no separator found")
        {
            ptrdiff_t idx = 0;
            auto result = kf::split(std::span{ kArr }, kMissingSeparator, idx);

            THEN("returns the whole array")
            {
                REQUIRE(result.size_bytes() == sizeof(kArr));
                REQUIRE(static_cast<const void*>(result.data()) == static_cast<const void*>(kArr));
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
                REQUIRE(kf::atOrDefault(std::span{ kArr }, 1, kDefault) == 8);
            }
        }

        WHEN("atOrDefault is called with an invalid index")
        {
            THEN("returns the default value")
            {
                REQUIRE(kf::atOrDefault(std::span{ kArr }, 5, kDefault) == kDefault);
            }
        }

        WHEN("atOrDefault is called on an empty span")
        {
            constexpr int* kEmpty = nullptr;

            THEN("returns the default value")
            {
                REQUIRE(kf::atOrDefault(std::span{ kEmpty, 0 }, 0, kOtherDefault) == kOtherDefault);
            }
        }
    }
}