#pragma once
#include <kf/stl/memory>

namespace kf
{
    class TimerThread
    {
    public:
        TimerThread()
        {
            KeInitializeEvent(&m_stopEvent, NotificationEvent, false);
        }

        TimerThread(const TimerThread&) = delete;
        TimerThread(TimerThread&&) = delete;

        ~TimerThread()
        {
            join();
        }

        TimerThread& operator=(TimerThread&&) = delete;
        TimerThread& operator=(const TimerThread&) = delete;

        NTSTATUS start(LARGE_INTEGER period, invocable<> auto&& callback)
        {
            // Need a negative value for KeWaitForSingleObject
            m_period.QuadPart = -period.QuadPart;

            using Context = pair<TimerThread*, decltype(callback)>;

            auto context = make_unique<Context, PagedPool>(this, move(callback));
            if (!context)
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            NTSTATUS status = startInternal([](PVOID rawContext)
                {
                    {
                        unique_ptr<Context> context{ static_cast<Context*>(rawContext) };
                        context->first->threadRoutine(context->second);
                    }

                    PsTerminateSystemThread(STATUS_SUCCESS);
                },
                context.get());
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            context.release();
            return STATUS_SUCCESS;
        }

        void join()
        {
            if (m_threadObject)
            {
                KeSetEvent(&m_stopEvent, IO_NO_INCREMENT, true);
                KeWaitForSingleObject(m_threadObject, Executive, KernelMode, false, nullptr);

                ObfDereferenceObject(m_threadObject);
                m_threadObject = nullptr;
            }
        }

    private:
        NTSTATUS startInternal(KSTART_ROUTINE routine, PVOID context)
        {
            ASSERT(!m_threadObject);

            OBJECT_ATTRIBUTES oa = RTL_CONSTANT_OBJECT_ATTRIBUTES(nullptr, OBJ_KERNEL_HANDLE);

            HANDLE threadHandle;
            NTSTATUS status = PsCreateSystemThread(&threadHandle,
                0,
                &oa,
                nullptr,
                nullptr,
                routine,
                context);
            if (!NT_SUCCESS(status))
            {
                return status;
            }
            SCOPE_EXIT{ ZwClose(threadHandle); };

            status = ObReferenceObjectByHandle(threadHandle, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, reinterpret_cast<PVOID*>(&m_threadObject), nullptr);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            return status;
        }

        void threadRoutine(invocable<> auto callback)
        {
            while (STATUS_TIMEOUT == KeWaitForSingleObject(&m_stopEvent, Executive, KernelMode, false, &m_period))
            {
                callback();
            }
        }

    private:
        PETHREAD m_threadObject{};
        KEVENT m_stopEvent{};
        LARGE_INTEGER m_period{};
    };
}
