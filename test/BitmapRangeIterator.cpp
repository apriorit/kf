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
    }
}
