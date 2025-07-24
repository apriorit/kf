#pragma once
#include <Ntddk.h>
#include <wdm.h>

namespace kf
{
    class RecursiveSpinLock
    {
    public:
        RecursiveSpinLock()
        {
            KeInitializeSpinLock(&m_spinLock);
        }

        bool lock(KIRQL& irql)
        {
            if (m_thread != PsGetCurrentThread())
            {
                KeAcquireSpinLock(&m_spinLock, &irql);
                m_thread = PsGetCurrentThread();

                return true;
            }

            return false;
        }

        void unlock(KIRQL& irql)
        {
            if (m_thread)
            {
                m_thread = nullptr;
                KeReleaseSpinLock(&m_spinLock, irql);
            }
        }

    private:
        RecursiveSpinLock(const RecursiveSpinLock&) = delete;
        RecursiveSpinLock& operator=(const RecursiveSpinLock&) = delete;

    private:
        PETHREAD   m_thread = nullptr;
        KSPIN_LOCK m_spinLock;
    };
}
