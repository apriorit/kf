#pragma once

namespace kf
{
    class WdfWaitAutoLock
    {
    public:
        WdfWaitAutoLock(WDFWAITLOCK lockHandle)
            : m_lockHandle(lockHandle)
        {
            ::WdfWaitLockAcquire(m_lockHandle, nullptr);
        }

        ~WdfWaitAutoLock()
        {
            ::WdfWaitLockRelease(m_lockHandle);
        }

    private:
        WdfWaitAutoLock(const WdfWaitAutoLock&) = delete;
        WdfWaitAutoLock& operator=(const WdfWaitAutoLock&) = delete;

    private:
        WDFWAITLOCK m_lockHandle;
    };
}
