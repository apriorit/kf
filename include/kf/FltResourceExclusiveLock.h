#pragma once
#include "FltResource.h"

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // FltResourceExclusiveLock

    class [[deprecated("Use std::unique_lock instead")]] FltResourceExclusiveLock
    {
    public:
        FltResourceExclusiveLock(_In_ PERESOURCE resource) : m_resource(FltResource::fromEResource(resource))
        {
            m_resource->fltAcquireExclusive();
        }

        FltResourceExclusiveLock(FltResourceExclusiveLock&& other) : m_resource(other.m_resource)
        {
            other.m_resource = nullptr;
        }

        ~FltResourceExclusiveLock()
        {
            if (m_resource)
            {
                m_resource->fltRelease();
            }
        }

    private:
        FltResourceExclusiveLock(const FltResourceExclusiveLock&);
        FltResourceExclusiveLock& operator=(const FltResourceExclusiveLock&);

    private:
        FltResource* m_resource;
    };
}
