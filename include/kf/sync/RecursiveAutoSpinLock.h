#pragma once
#include <wdm.h>

#include "RecursiveSpinLock.h"

namespace kf
{
    class RecursiveAutoSpinLock
    {
    public:
        explicit RecursiveAutoSpinLock(_In_ RecursiveSpinLock& spinLock)
            : m_spinLock(spinLock)
            , m_isLocked(m_spinLock.lock(m_oldIrql))
        {
        }

        ~RecursiveAutoSpinLock()
        {
            if (m_isLocked)
            {
                m_spinLock.unlock(m_oldIrql);
            }
        }

    private:
        RecursiveAutoSpinLock(const AutoSpinLock&) = delete;
        RecursiveAutoSpinLock& operator=(const AutoSpinLock&) = delete;

    private:
        RecursiveSpinLock& m_spinLock;
        bool               m_isLocked = false;
        KIRQL              m_oldIrql;
    };
}
