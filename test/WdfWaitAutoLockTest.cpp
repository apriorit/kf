#include "pch.h"
#include <kf/WdfWaitLock.h>
#include <kf/WdfWaitAutoLock.h>
#include <kf/Thread.h>

SCENARIO("WdfWaitAutoLock ensures exclusive access to shared data")
{
    GIVEN("A WDFWAITLOCK and shared data")
    {
        struct Context
        {
            kf::WdfWaitLock* lock = nullptr;
            std::array<char, 2>* buffer = nullptr;
        };
        kf::WdfWaitLock wdfLock;
        wdfLock.init();
        std::array<char, 2> buffer{};
		Context context{ &wdfLock, &buffer };
        
        kf::Thread thread;

        WHEN("First thread holds the lock and writes to the buffer")
        {
            {
                kf::WdfWaitAutoLock lock(wdfLock);
                context.buffer->at(0) = 'a';

                thread.start([](void* context) {
                    auto ctx = static_cast<Context*>(context);
                    kf::WdfWaitAutoLock lock(*ctx->lock);
                    ctx->buffer->at(1) = 'b';
                    }, &context);

                constexpr int kOneMillisecond = 10'000; // 1 ms in 100-nanosecond intervals
                LARGE_INTEGER interval;
                interval.QuadPart = -kOneMillisecond;
                KeDelayExecutionThread(KernelMode, FALSE, &interval);

                THEN("Only 'a' is added to the array")
                {
                    REQUIRE(buffer[0] == 'a');
                    REQUIRE(buffer[0] == '0');
                }
                thread.join();
            }

            THEN("First lock is out of scope and second thread can acquire the lock and write to the buffer")
            {
                REQUIRE(buffer[0] == 'a');
                REQUIRE(buffer[0] == 'b');
            }
        }
    }
}
