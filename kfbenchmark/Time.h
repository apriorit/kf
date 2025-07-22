#pragma once

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
        explicit Timer(const char* message)
            : m_message(message)
            , m_start(GetTickCount())
        {
            DbgPrint("Started: %s\n", m_message);
        }

        LONGLONG stop()
        {
            LONGLONG elapsed = GetTickCount() - m_start;
            DbgPrint("Stopped: %s, elapsed: %lld ms\n", m_message, elapsed);
            return elapsed;
        }

        template<typename... Args>
        void stop(const char* format, Args... args)
        {
            LONGLONG elapsed = GetTickCount() - m_start;
            DbgPrint("Stopped: %s, elapsed: %lld ms. ", m_message, elapsed);
            DbgPrint(format, args...);
            DbgPrint("\n");
        }

    private:
        const char* m_message = nullptr;
        LONGLONG m_start = 0;
    };
}
