#pragma once

namespace kf
{
    class Event
    {
    public:
        Event(EVENT_TYPE type, bool state)
        {
            KeInitializeEvent(&m_event, type, state);
        }

        operator PKEVENT()
        {
            return &m_event;
        }

        void set()
        {
            KeSetEvent(&m_event, IO_NO_INCREMENT, false);
        }

        void clear()
        {
            KeClearEvent(&m_event);
        }

        bool isSet()
        {
            return !!KeReadStateEvent(&m_event);
        }

        NTSTATUS wait(_In_opt_ PLARGE_INTEGER timeout = nullptr)
        {
            ASSERT(KeGetCurrentIrql() <= APC_LEVEL);
            return KeWaitForSingleObject(&m_event, Executive, KernelMode, false, timeout);
        }

    private:
        Event(const Event&);
        Event& operator= (const Event&);

    private:
        KEVENT m_event;
    };
}
