#pragma once
#include "EResource.h"
#include "Semaphore.h"

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // ConditionVariable

    class ConditionVariable
    {
    public:
        ConditionVariable() : m_semaphore(0, MAXLONG)
        {
        }

        ConditionVariable(const ConditionVariable&) = delete;
        ConditionVariable& operator=(const ConditionVariable&) = delete;

        enum class Status
        {
            Timeout = 0,
            Success
        };

        void wait(EResource& external)
        {
            waitFor(external, nullptr);
        }

        template<class Predicate>
        void wait(EResource& external, Predicate predicate)
        {
            waitFor(external, nullptr, predicate);
        }

        Status waitFor(EResource& external, PLARGE_INTEGER timeout)
        {
            ++m_waitersCount;

            external.release();
            auto status = NT_SUCCESS(m_semaphore.wait(timeout)) ? Status::Success : Status::Timeout;
            external.acquireExclusive();

            --m_waitersCount;

            return status;
        }

        template<class Predicate>
        bool waitFor(EResource& external, PLARGE_INTEGER timeout, Predicate predicate)
        {
            while (!predicate())
            {
                if (waitFor(external, timeout) == Status::Timeout)
                {
                    return predicate();
                }
            }

            return true;
        }

        void notifyOne()
        {
            auto waitersCount = m_waitersCount.load();
            if (waitersCount > 0)
            {
                m_semaphore.release();
            }
        }

        void notifyAll()
        {
            auto waitersCount = m_waitersCount.load();
            if (waitersCount > 0)
            {
                m_semaphore.release(waitersCount);
            }
        }

    private:
        std::atomic_long m_waitersCount = 0;
        Semaphore m_semaphore;
    };
}
