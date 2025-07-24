#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // FltResourceSharedLock

    class [[deprecated("Use std::unique_lock instead")]] FltResourceSharedLock
    {
    public:
        FltResourceSharedLock(_In_ PERESOURCE resource) : m_resource(FltResource::fromEResource(resource))
        {
            m_resource->fltAcquireShared();
        }

        ~FltResourceSharedLock()
        {
            m_resource->fltRelease();
        }

    private:
        FltResourceSharedLock(const FltResourceSharedLock&);
        FltResourceSharedLock& operator=(const FltResourceSharedLock&);

    private:
        FltResource* m_resource;
    };
}
