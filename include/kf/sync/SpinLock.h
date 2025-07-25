#pragma once
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
