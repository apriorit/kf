#pragma once
#include "kf/stl/memory"
#include "kf/guard/ScopeFailure.h"

namespace kf
{
    namespace detail
    {
        template<typename T>
        class FltWorkItemWorker
        {
        public:
            FltWorkItemWorker(T&& routine) : m_routine(routine)
            {
            }

            NTSTATUS queue(PFLT_FILTER filter)
            {
                auto fltWorkItem = FltAllocateGenericWorkItem();
                if (!fltWorkItem)
                {
                    return STATUS_INSUFFICIENT_RESOURCES;
                }

                NTSTATUS status = STATUS_SUCCESS;
                SCOPE_FAILURE(status) { FltFreeGenericWorkItem(fltWorkItem); };

                status = FltQueueGenericWorkItem(fltWorkItem, filter, &workItemRoutine, DelayedWorkQueue, this);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }

                return STATUS_SUCCESS;
            }

        private:
            static void workItemRoutine(PFLT_GENERIC_WORKITEM fltWorkItem, PVOID, PVOID context)
            {
                FltFreeGenericWorkItem(fltWorkItem);

                auto self = static_cast<FltWorkItemWorker*>(context);
                self->m_routine();
                delete self;
            }

        private:
            T m_routine;
        };
    }

    class FltWorkItem
    {
    public:
        template<typename T>
        static NTSTATUS queue(PFLT_FILTER filter, T&& routine)
        {
            auto item = make_unique<detail::FltWorkItemWorker<T>, PagedPool>(move(routine));
            if (!item)
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            auto status = item->queue(filter);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            item.release();
            return STATUS_SUCCESS;
        }
    };
} // namespace
