#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // Semaphore

    class Semaphore
    {
    public:
        Semaphore(LONG count, LONG limit)
        {
            KeInitializeSemaphore(&m_semaphore, count, limit);
        }

        Semaphore(const Semaphore&) = delete;
        Semaphore& operator=(const Semaphore&) = delete;

        operator PKSEMAPHORE()
        {
            return &m_semaphore;
        }

        NTSTATUS wait(_In_opt_ const LARGE_INTEGER* timeout = nullptr)
        {
            ASSERT(KeGetCurrentIrql() <= APC_LEVEL);
            return KeWaitForSingleObject(&m_semaphore, Executive, KernelMode, false, const_cast<LARGE_INTEGER*>(timeout));
        }

        void release(LONG adjustment = 1)
        {
            KeReleaseSemaphore(&m_semaphore, IO_NO_INCREMENT, adjustment, false);
        }

    private:
        KSEMAPHORE m_semaphore;
    };
}
