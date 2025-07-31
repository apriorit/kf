#include "pch.h"
#include <kf/Bitmap.h>

SCENARIO("BitmapRangeIterator")
{
    GIVEN("initialized Bitmap with size 10")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(10));

        auto iterator = bitmap.rangeIterator();

        WHEN("do nothing, bitmap is [0,0,0,0,0,0,0,0,0,0]")
        {
            THEN("iterator returs no ranges")
            {
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [1,0,0,0,0,0,0,0,0,0]")
        {
            bitmap.setBits(0, 1);

            THEN("iterator returs ranges: {0,1}")
            {
                REQUIRE(*iterator.next() == std::make_pair(0UL, 1UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [1,1,1,0,0,0,0,0,0,0]")
        {
            bitmap.setBits(0, 3);

            THEN("iterator returs ranges: {0,3}")
            {
                REQUIRE(*iterator.next() == std::make_pair(0UL, 3UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [1,0,1,0,1,0,0,0,0,0]")
        {
            bitmap.setBits(0, 1);
            bitmap.setBits(2, 1);
            bitmap.setBits(4, 1);

            THEN("iterator returns ranges: {0,1}, {2,1}, {4,1}")
            {
                REQUIRE(*iterator.next() == std::make_pair(0UL, 1UL));
                REQUIRE(*iterator.next() == std::make_pair(2UL, 1UL));
                REQUIRE(*iterator.next() == std::make_pair(4UL, 1UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [0,0,0,0,0,0,0,1,1,1]")
        {
            bitmap.setBits(7, 3);

            THEN("iterator returns ranges: {7,3}")
            {
                REQUIRE(*iterator.next() == std::make_pair(7UL, 3UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [1,1,1,1,1,1,1,1,1,1]")
        {
            bitmap.setBits(0, 10);

            THEN("iterator returns ranges: {0,10}")
            {
                REQUIRE(*iterator.next() == std::make_pair(0UL, 10UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [1,1,0,0,1,1,1,0,0,1]")
        {
            bitmap.setBits(0, 2);
            bitmap.setBits(4, 3);
            bitmap.setBits(9, 1);

            THEN("iterator returns ranges: {0,2}, {4,3}, {9,1}")
            {
                REQUIRE(*iterator.next() == std::make_pair(0UL, 2UL));
                REQUIRE(*iterator.next() == std::make_pair(4UL, 3UL));
                REQUIRE(*iterator.next() == std::make_pair(9UL, 1UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [0,0,0,0,0,0,0,0,0,1]")
        {
            bitmap.setBits(9, 1);

            THEN("iterator returns ranges: {9,1}")
            {
                REQUIRE(*iterator.next() == std::make_pair(9UL, 1UL));
                REQUIRE(!iterator.next());
            }
        }
    }

    GIVEN("initialized Bitmap with size 32")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(32));

        auto iterator = bitmap.rangeIterator();

        WHEN("set alternating bits [1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0]")
        {
            for (ULONG i = 0; i < 32; i += 2)
            {
                bitmap.setBits(i, 1);
            }

            THEN("iterator returns 16 ranges of single bits")
            {
                for (ULONG i = 0; i < 32; i += 2)
                {
                    REQUIRE(*iterator.next() == std::make_pair(i, 1UL));
                }
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set large contiguous range [0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,0,0,0,0]")
        {
            bitmap.setBits(4, 24);

            THEN("iterator returns ranges: {4,24}")
            {
                REQUIRE(*iterator.next() == std::make_pair(4UL, 24UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bits with large gaps [1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1]")
        {
            bitmap.setBits(0, 1);
            bitmap.setBits(10, 1);
            bitmap.setBits(20, 1);
            bitmap.setBits(31, 1);

            THEN("iterator returns ranges: {0,1}, {10,1}, {20,1}, {31,1}")
            {
                REQUIRE(*iterator.next() == std::make_pair(0UL, 1UL));
                REQUIRE(*iterator.next() == std::make_pair(10UL, 1UL));
                REQUIRE(*iterator.next() == std::make_pair(20UL, 1UL));
                REQUIRE(*iterator.next() == std::make_pair(31UL, 1UL));
                REQUIRE(!iterator.next());
            }
        }
    }

    GIVEN("initialized Bitmap with size 10 and iterator with custom starting position")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(10));

        WHEN("set bitmap to [1,1,0,1,1,0,1,1,1,0] and start iterator from index 3")
        {
            bitmap.setBits(0, 2);
            bitmap.setBits(3, 2);
            bitmap.setBits(6, 3);

            auto iterator = bitmap.rangeIterator(3);

            THEN("iterator returns ranges starting from index 3: {3,2}, {6,3}")
            {
                REQUIRE(*iterator.next() == std::make_pair(3UL, 2UL));
                REQUIRE(*iterator.next() == std::make_pair(6UL, 3UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bitmap to [1,1,1,1,1,1,1,1,1,1] and start iterator from index 5")
        {
            bitmap.setBits(0, 10);

            auto iterator = bitmap.rangeIterator(5);

            THEN("iterator returns ranges starting from index 5: {5,5}")
            {
                REQUIRE(*iterator.next() == std::make_pair(5UL, 5UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("start iterator from index beyond bitmap size")
        {
            bitmap.setBits(0, 10);

            auto iterator = bitmap.rangeIterator(15);

            THEN("iterator returns no ranges")
            {
                REQUIRE(!iterator.next());
            }
        }

        WHEN("start iterator from last index with bit set")
        {
            bitmap.setBits(9, 1);

            auto iterator = bitmap.rangeIterator(9);

            THEN("iterator returns ranges: {9,1}")
            {
                REQUIRE(*iterator.next() == std::make_pair(9UL, 1UL));
                REQUIRE(!iterator.next());
            }
        }
    }

    GIVEN("edge cases with different bitmap patterns")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(16));

        WHEN("set only first and last bits [1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1]")
        {
            bitmap.setBits(0, 1);
            bitmap.setBits(15, 1);

            auto iterator = bitmap.rangeIterator();

            THEN("iterator returns ranges: {0,1}, {15,1}")
            {
                REQUIRE(*iterator.next() == std::make_pair(0UL, 1UL));
                REQUIRE(*iterator.next() == std::make_pair(15UL, 1UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set bits creating two adjacent ranges [0,0,1,1,1,1,0,0,0,0,0,0,0,0,0,0]")
        {
            bitmap.setBits(2, 4);

            auto iterator = bitmap.rangeIterator();

            THEN("iterator returns single range: {2,4}")
            {
                REQUIRE(*iterator.next() == std::make_pair(2UL, 4UL));
                REQUIRE(!iterator.next());
            }
        }

        WHEN("set complex pattern [0,1,1,0,0,1,0,1,1,1,0,0,1,1,0,0]")
        {
            bitmap.setBits(1, 2);
            bitmap.setBits(5, 1);
            bitmap.setBits(7, 3);
            bitmap.setBits(12, 2);

            auto iterator = bitmap.rangeIterator();

            THEN("iterator returns ranges: {1,2}, {5,1}, {7,3}, {12,2}")
            {
                REQUIRE(*iterator.next() == std::make_pair(1UL, 2UL));
                REQUIRE(*iterator.next() == std::make_pair(5UL, 1UL));
                REQUIRE(*iterator.next() == std::make_pair(7UL, 3UL));
                REQUIRE(*iterator.next() == std::make_pair(12UL, 2UL));
                REQUIRE(!iterator.next());
            }
        }
    }
}
