#pragma once

namespace kf
{
    class WdfWaitLock
    {
    public:
        WdfWaitLock()
            : m_lockHandle(nullptr)
        {
        }

        ~WdfWaitLock()
        {
            if (m_lockHandle)
            {
                ::WdfObjectDelete(m_lockHandle);
                m_lockHandle = nullptr;
            }
        }
        operator WDFWAITLOCK()
        {
            return m_lockHandle;
        }

        NTSTATUS init()
        {
            return ::WdfWaitLockCreate(WDF_NO_OBJECT_ATTRIBUTES, &m_lockHandle);
        }

        WdfWaitLock& operator=(WdfWaitLock&& tmp)
        {
            m_lockHandle = tmp.m_lockHandle;
            tmp.m_lockHandle = nullptr;

            return *this;
        }

    private:
        WdfWaitLock(const WdfWaitLock&) = delete;
        WdfWaitLock& operator=(const WdfWaitLock&) = delete;

    private:
        WDFWAITLOCK m_lockHandle;
    };
}
