#include "pch.h"
#include "kf/Map.h"
#include "Time.h"

using namespace kftest;

SCENARIO("Find benchmark")
{
    kf::Map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    static const int kMaxKey = 10000000;

    for (int i = 0; i < kMaxKey; ++i)
    {
        if (i % 100000 == 0)
        {
            DbgPrint("Populating: %d/%d\n", i, kMaxKey);
        }
        auto result = map.emplace(i, i * 10);
        REQUIRE(result.has_value());
        REQUIRE(result.value().second);
    }

    Timer timer("Map find benchmark");
    for (int i = 0; i < kMaxKey; ++i)
    {
        if (i % 100000 == 0)
        {
            DbgPrint("Finding: %d/%d\n", i, kMaxKey);
        }

        auto value = map.find(i);
        REQUIRE(value != map.end());
        REQUIRE((*value).second == i * 10);
    }

    auto ms = timer.stop();
    KdPrint(("Finding speed: %d/s\n", (kMaxKey * 1000) / ms));
}