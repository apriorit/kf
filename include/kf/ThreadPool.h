#pragma once
#include "Thread.h"

namespace kf
{
    template<int kMaxCount = 64>
    class ThreadPool
    {
    public:
        ThreadPool(int count) noexcept : m_count((std::min)(count, kMaxCount))
        {
        }

        ThreadPool(ThreadPool&&) noexcept = default;

        ~ThreadPool()
        {
            join();
        }

        ThreadPool& operator=(ThreadPool&&) noexcept = default;

        NTSTATUS start(KSTART_ROUTINE routine, PVOID context)
        {
            for (int i = 0; i < m_count; ++i)
            {
                NTSTATUS status = m_threads[i].start(routine, context);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }
            }

            return STATUS_SUCCESS;
        }

        // routine sould be NTSTATUS (T::*)()
        template<auto routine, class T>
        NTSTATUS start(T* obj)
        {
            return start([](PVOID context) { PsTerminateSystemThread((static_cast<T*>(context)->*routine)()); }, obj);
        }

        void join()
        {
            for (int i = 0; i < m_count; ++i)
            {
                m_threads[i].join();
            }
        }

    private:
        Thread m_threads[kMaxCount];
        int m_count;
    };
}
