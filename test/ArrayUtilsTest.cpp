#include "pch.h"
#include <kf/ArrayUtils.h>

using namespace kf;

SCENARIO("ArrayUtils: makeArrayOfBytes")
{
    GIVEN("A set of integer values")
    {
        auto arr = makeArrayOfBytes(0x12, 0x34, 0x56);

        THEN("The resulting array contains the corresponding std::byte values")
        {
            REQUIRE(arr.size() == 3);
            REQUIRE(arr[0] == std::byte{0x12});
            REQUIRE(arr[1] == std::byte{0x34});
            REQUIRE(arr[2] == std::byte{0x56});
        }
    }

    GIVEN("No arguments")
    {
        auto arr = makeArrayOfBytes();

        THEN("The resulting array is empty")
        {
            REQUIRE(arr.size() == 0);
        }
    }
}