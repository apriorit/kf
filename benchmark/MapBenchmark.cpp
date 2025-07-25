#include "pch.h"
#include "kf/Map.h"
#include "Time.h"

using namespace kftest;

namespace
{
    void PrintProgress(int current, int total)
    {
        if (current % 100000 == 0)
        {
            DbgPrint("%d/%d\n", current, total);
        }
    }
}

SCENARIO("Map benchmark: populate and find")
{
    kf::Map<int, int, NonPagedPoolNx> map;
    REQUIRE_NT_SUCCESS(map.initialize());

    static const int kMaxKey = 10000000;

    Timer timer("populating");
    for (int i = 0; i < kMaxKey; ++i)
    {
        PrintProgress(i, kMaxKey);

        auto result = map.emplace(i, i * 10);
        REQUIRE(result.has_value());
        REQUIRE(result.value().second);
    }
    auto ms = timer.stop();
    KdPrint(("Speed is: %d elements/s\n", (kMaxKey * 1000) / ms));

    timer.reset("finding");
    for (int i = 0; i < kMaxKey; ++i)
    {
        PrintProgress(i, kMaxKey);

        auto value = map.find(i);
        REQUIRE(value != map.end());
        REQUIRE((*value).second == i * 10);
    }

    ms = timer.stop();
    KdPrint(("Speed is: %d elements/s\n", (kMaxKey * 1000) / ms));
}