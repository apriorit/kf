#pragma once
// WIN9X_COMPAT_SPINLOCK must be defined to ensure binary compatibility with Vista and Windows 7
#ifndef WIN9X_COMPAT_SPINLOCK
    #define WIN9X_COMPAT_SPINLOCK
#endif
#include <wdm.h>

namespace kf
{
    class SpinLock
    {
    public:
        SpinLock()
        {
            KeInitializeSpinLock(&m_spinLock);
        }

        SpinLock(SpinLock&&)
        {
            KeInitializeSpinLock(&m_spinLock);
        }

        SpinLock& operator=(SpinLock&&)
        {
            return *this;
        }

        operator PKSPIN_LOCK()
        {
            return &m_spinLock;
        }

    private:
        SpinLock(const SpinLock&) = delete;
        SpinLock& operator=(const SpinLock&) = delete;

    private:
        KSPIN_LOCK m_spinLock;
    };
}
