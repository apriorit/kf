#pragma once
#include <wdm.h>

namespace kf
{
    class AutoSpinLock
    {
    public:
        AutoSpinLock(_In_ PKSPIN_LOCK spinLock)
            : m_spinLock(spinLock)
        {
            KeAcquireSpinLock(m_spinLock, &m_oldIrql);
        }

        ~AutoSpinLock()
        {
            KeReleaseSpinLock(m_spinLock, m_oldIrql);
        }

    private:
        AutoSpinLock(const AutoSpinLock&) = delete;
        AutoSpinLock& operator=(const AutoSpinLock&) = delete;

    private:
        PKSPIN_LOCK m_spinLock;
        KIRQL       m_oldIrql;
    };
}
