#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // Semaphore

    class Semaphore
    {
    public:
        Semaphore(LONG count, LONG limit) : m_limit(limit)
        {
            KeInitializeSemaphore(&m_semaphore, count, limit);
        }

        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;

        operator PKSEMAPHORE()
        {
            return &m_semaphore;
        }

        NTSTATUS wait(_In_opt_ PLARGE_INTEGER timeout = nullptr)
        {
            ASSERT(KeGetCurrentIrql() <= APC_LEVEL);
            return KeWaitForSingleObject(&m_semaphore, Executive, KernelMode, false, timeout);
        }

        void release(LONG adjustment = 1)
        {
            LONG currentCount = KeReadStateSemaphore(&m_semaphore);
            if (currentCount + adjustment > m_limit)
            {
                return;
            }
            KeReleaseSemaphore(&m_semaphore, IO_NO_INCREMENT, adjustment, false);
        }

    private:
        LONG m_limit = 0;
        KSEMAPHORE m_semaphore;
    };
}
