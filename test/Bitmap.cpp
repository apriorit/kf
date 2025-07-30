#include "pch.h"
#include <kf/Bitmap.h>

SCENARIO("Bitmap")
{
    GIVEN("initialized bitmap with size 10")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(10));

        WHEN("do nothing")
        {
            THEN("bitmap size is 10")
            {
                REQUIRE(bitmap.size() == 10);
            }

            THEN("each bit is clear")
            {
                for (int i = 0; i < 10; ++i)
                {
                    REQUIRE(bitmap.areBitsClear(i, 1));
                }
            }

            THEN("no bit is set")
            {
                for (int i = 0; i < 10; ++i)
                {
                    REQUIRE(!bitmap.areBitsSet(i, 1));
                }
            }
        }
    }
}
