#pragma once
#include "kf/guard/ObjectAttributes.h"
#include "kf/guard/Guard.h"

namespace kf
{
    class Thread
    {
    public:
        Thread() = default;
        Thread(Thread&&) = default;

        ~Thread()
        {
            join();
        }

        Thread& operator=(Thread&&) = default;

        Thread(const Thread&) = delete;
        Thread& operator=(const Thread&) = delete;

        NTSTATUS start(KSTART_ROUTINE routine, PVOID context)
        {
            ASSERT(!m_threadObject);

            ObjectAttributes oa;

            Guard::Handle threadHandle;
            NTSTATUS status = PsCreateSystemThread(&threadHandle.get(),
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

            status = ObReferenceObjectByHandle(threadHandle, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, reinterpret_cast<PVOID*>(&m_threadObject.get()), nullptr);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            return status;
        }

        // routine sould be NTSTATUS (T::*)()
        template<auto routine, class T>
        NTSTATUS start(T* obj)
        {
            return start([](PVOID context) { PsTerminateSystemThread((static_cast<T*>(context)->*routine)()); }, obj);
        }

        void join()
        {
            if (m_threadObject)
            {
                KeWaitForSingleObject(m_threadObject, Executive, KernelMode, false, nullptr);
                m_threadObject.reset();
            }
        }

    private:
        Guard::EThread m_threadObject;
    };
}
