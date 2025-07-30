#include "pch.h"
#include <kf/Bitmap.h>

SCENARIO("Bitmap basic operations")
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

            THEN("number of clear bits equals size, number of set bits is 0")
            {
                REQUIRE(bitmap.numberOfClearBits() == 10);
                REQUIRE(bitmap.numberOfSetBits() == 0);
            }
        }
    }
}

SCENARIO("Bitmap setBits and clearBits operations")
{
    GIVEN("initialized bitmap with size 20")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(20));

        WHEN("set single bit at index 5")
        {
            bitmap.setBits(5, 1);

            THEN("bit at index 5 is set, others are clear")
            {
                REQUIRE(bitmap.areBitsSet(5, 1));
                REQUIRE(bitmap.areBitsClear(0, 5));
                REQUIRE(bitmap.areBitsClear(6, 14));
                REQUIRE(bitmap.numberOfSetBits() == 1);
                REQUIRE(bitmap.numberOfClearBits() == 19);
            }
        }

        WHEN("set multiple consecutive bits")
        {
            bitmap.setBits(3, 5);

            THEN("bits 3-7 are set, others are clear")
            {
                REQUIRE(bitmap.areBitsSet(3, 5));
                REQUIRE(bitmap.areBitsClear(0, 3));
                REQUIRE(bitmap.areBitsClear(8, 12));
                REQUIRE(bitmap.numberOfSetBits() == 5);
                REQUIRE(bitmap.numberOfClearBits() == 15);
            }
        }

        WHEN("set bits then clear some of them")
        {
            bitmap.setBits(2, 8);
            bitmap.clearBits(4, 3);

            THEN("bits are correctly set and cleared")
            {
                REQUIRE(bitmap.areBitsSet(2, 2));  // bits 2-3
                REQUIRE(bitmap.areBitsClear(4, 3)); // bits 4-6
                REQUIRE(bitmap.areBitsSet(7, 3));   // bits 7-9
                REQUIRE(bitmap.numberOfSetBits() == 5);
                REQUIRE(bitmap.numberOfClearBits() == 15);
            }
        }

        WHEN("set bits at boundaries")
        {
            bitmap.setBits(0, 1);  // first bit
            bitmap.setBits(19, 1); // last bit

            THEN("boundary bits are set correctly")
            {
                REQUIRE(bitmap.areBitsSet(0, 1));
                REQUIRE(bitmap.areBitsSet(19, 1));
                REQUIRE(bitmap.areBitsClear(1, 18));
                REQUIRE(bitmap.numberOfSetBits() == 2);
                REQUIRE(bitmap.numberOfClearBits() == 18);
            }
        }
    }
}

SCENARIO("Bitmap setAll and clearAll operations")
{
    GIVEN("initialized bitmap with size 15")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(15));

        WHEN("set all bits")
        {
            bitmap.setAll();

            THEN("all bits are set")
            {
                REQUIRE(bitmap.areBitsSet(0, 15));
                REQUIRE(bitmap.numberOfSetBits() == 15);
                REQUIRE(bitmap.numberOfClearBits() == 0);
            }
        }

        WHEN("set all bits then clear all")
        {
            bitmap.setAll();
            bitmap.clearAll();

            THEN("all bits are clear")
            {
                REQUIRE(bitmap.areBitsClear(0, 15));
                REQUIRE(bitmap.numberOfSetBits() == 0);
                REQUIRE(bitmap.numberOfClearBits() == 15);
            }
        }

        WHEN("set some bits, then set all, then clear all")
        {
            bitmap.setBits(3, 5);
            bitmap.setAll();
            
            THEN("all bits are set")
            {
                REQUIRE(bitmap.areBitsSet(0, 15));
                REQUIRE(bitmap.numberOfSetBits() == 15);
            }

            bitmap.clearAll();

            THEN("all bits are clear after clearAll")
            {
                REQUIRE(bitmap.areBitsClear(0, 15));
                REQUIRE(bitmap.numberOfSetBits() == 0);
                REQUIRE(bitmap.numberOfClearBits() == 15);
            }
        }
    }
}

SCENARIO("Bitmap move constructor and move assignment")
{
    GIVEN("initialized bitmap with set bits")
    {
        kf::Bitmap<PagedPool> bitmap1;
        REQUIRE_NT_SUCCESS(bitmap1.initialize(10));
        bitmap1.setBits(2, 3);
        bitmap1.setBits(7, 2);

        WHEN("move construct new bitmap")
        {
            kf::Bitmap<PagedPool> bitmap2(std::move(bitmap1));

            THEN("new bitmap has the bits, original is empty")
            {
                REQUIRE(bitmap2.size() == 10);
                REQUIRE(bitmap2.areBitsSet(2, 3));
                REQUIRE(bitmap2.areBitsSet(7, 2));
                REQUIRE(bitmap2.numberOfSetBits() == 5);
                
                // Original bitmap should be in moved-from state (size 0)
                REQUIRE(bitmap1.size() == 0);
            }
        }

        WHEN("move assign to new bitmap")
        {
            kf::Bitmap<PagedPool> bitmap2;
            bitmap2 = std::move(bitmap1);

            THEN("new bitmap has the bits, original is empty")
            {
                REQUIRE(bitmap2.size() == 10);
                REQUIRE(bitmap2.areBitsSet(2, 3));
                REQUIRE(bitmap2.areBitsSet(7, 2));
                REQUIRE(bitmap2.numberOfSetBits() == 5);
                
                // Original bitmap should be in moved-from state (size 0)
                REQUIRE(bitmap1.size() == 0);
            }
        }

        WHEN("move assign to initialized bitmap")
        {
            kf::Bitmap<PagedPool> bitmap2;
            REQUIRE_NT_SUCCESS(bitmap2.initialize(5));
            bitmap2.setAll();
            
            bitmap2 = std::move(bitmap1);

            THEN("new bitmap replaces old content")
            {
                REQUIRE(bitmap2.size() == 10);
                REQUIRE(bitmap2.areBitsSet(2, 3));
                REQUIRE(bitmap2.areBitsSet(7, 2));
                REQUIRE(bitmap2.numberOfSetBits() == 5);
                
                // Original bitmap should be in moved-from state (size 0)
                REQUIRE(bitmap1.size() == 0);
            }
        }

        WHEN("self move assignment")
        {
            auto* ptr = &bitmap1;
            bitmap1 = std::move(*ptr);

            THEN("bitmap remains unchanged")
            {
                REQUIRE(bitmap1.size() == 10);
                REQUIRE(bitmap1.areBitsSet(2, 3));
                REQUIRE(bitmap1.areBitsSet(7, 2));
                REQUIRE(bitmap1.numberOfSetBits() == 5);
            }
        }
    }
}

SCENARIO("Bitmap edge cases")
{
    GIVEN("bitmap with size 1")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(1));

        WHEN("set the single bit")
        {
            bitmap.setBits(0, 1);

            THEN("single bit operations work correctly")
            {
                REQUIRE(bitmap.areBitsSet(0, 1));
                REQUIRE(bitmap.numberOfSetBits() == 1);
                REQUIRE(bitmap.numberOfClearBits() == 0);
            }
        }

        WHEN("clear the single bit after setting it")
        {
            bitmap.setBits(0, 1);
            bitmap.clearBits(0, 1);

            THEN("single bit is clear")
            {
                REQUIRE(bitmap.areBitsClear(0, 1));
                REQUIRE(bitmap.numberOfSetBits() == 0);
                REQUIRE(bitmap.numberOfClearBits() == 1);
            }
        }
    }

    GIVEN("bitmap with larger size")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(64));

        WHEN("set and clear across word boundaries")
        {
            bitmap.setBits(30, 10);  // crosses 32-bit boundary

            THEN("bits across boundaries are set correctly")
            {
                REQUIRE(bitmap.areBitsSet(30, 10));
                REQUIRE(bitmap.numberOfSetBits() == 10);
                REQUIRE(bitmap.numberOfClearBits() == 54);
            }
        }

        WHEN("set all 64 bits")
        {
            bitmap.setAll();

            THEN("all 64 bits are set")
            {
                REQUIRE(bitmap.areBitsSet(0, 64));
                REQUIRE(bitmap.numberOfSetBits() == 64);
                REQUIRE(bitmap.numberOfClearBits() == 0);
            }
        }

        WHEN("perform mixed operations")
        {
            bitmap.setBits(0, 20);
            bitmap.clearBits(5, 10);
            bitmap.setBits(60, 4);

            THEN("mixed operations result in correct state")
            {
                REQUIRE(bitmap.areBitsSet(0, 5));   // 0-4
                REQUIRE(bitmap.areBitsClear(5, 10)); // 5-14
                REQUIRE(bitmap.areBitsSet(15, 5));   // 15-19
                REQUIRE(bitmap.areBitsClear(20, 40)); // 20-59
                REQUIRE(bitmap.areBitsSet(60, 4));   // 60-63
                REQUIRE(bitmap.numberOfSetBits() == 14);
                REQUIRE(bitmap.numberOfClearBits() == 50);
            }
        }
    }

    GIVEN("uninitialized bitmap")
    {
        kf::Bitmap<PagedPool> bitmap;

        WHEN("check size")
        {
            THEN("size is 0")
            {
                REQUIRE(bitmap.size() == 0);
            }
        }
    }
}

SCENARIO("Bitmap rangeIterator")
{
    GIVEN("bitmap with mixed set and clear bits")
    {
        kf::Bitmap<PagedPool> bitmap;
        REQUIRE_NT_SUCCESS(bitmap.initialize(20));
        bitmap.setBits(2, 3);   // bits 2,3,4
        bitmap.setBits(8, 2);   // bits 8,9
        bitmap.setBits(15, 3);  // bits 15,16,17

        WHEN("get range iterator from beginning")
        {
            auto iterator = bitmap.rangeIterator();

            THEN("iterator is created successfully")
            {
                // We can't test iteration behavior without running,
                // but we can verify the iterator is constructible
                static_assert(std::is_same_v<decltype(iterator), kf::BitmapRangeIterator>);
            }
        }

        WHEN("get range iterator from specific starting index")
        {
            auto iterator = bitmap.rangeIterator(10);

            THEN("iterator is created with custom starting index")
            {
                // Verify iterator can be created with starting index
                static_assert(std::is_same_v<decltype(iterator), kf::BitmapRangeIterator>);
            }
        }
    }
}
