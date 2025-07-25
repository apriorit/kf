#pragma once
#include <sal.h>

namespace kftest
{
    /*
    * GetTickCountPrecise returns system time in microseconds
    */
    _Must_inspect_result_
    inline LONGLONG GetTickCountPrecise()
    {
        LARGE_INTEGER systemTime;
        KeQuerySystemTimePrecise(&systemTime);
        // systemTime contains count of 100ns intervals, so /10 gives microseconds
        return systemTime.QuadPart / 10;
    }

    /*
    * GetTickCount returns system time in milliseconds
    */
    _Must_inspect_result_
    inline LONGLONG GetTickCount()
    {
        // microseconds / 1000 = milliseconds
        return GetTickCountPrecise() / 1000;
    }

    class Timer
    {
    public:
        explicit Timer(const char* stage)
        {
            reset(stage);
        }

        void reset(const char* stage)
        {
            m_stageName = stage;
            m_start = GetTickCount();
            DbgPrint("Started %s\n", m_stageName);
        }

        LONGLONG stop()
        {
            LONGLONG elapsed = GetTickCount() - m_start;
            DbgPrint("Stopped %s, elapsed: %lld ms\n", m_stageName, elapsed);
            return elapsed;
        }

    private:
        const char* m_stageName = nullptr;
        LONGLONG m_start = 0;
    };
}
