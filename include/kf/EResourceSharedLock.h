#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // EResourceSharedLock

    class EResourceSharedLock
    {
    public:
        EResourceSharedLock(_In_ EResource& resource) : m_resource(resource)
        {
            KeEnterCriticalRegion();
            m_resource.acquireShared();
        }

        ~EResourceSharedLock()
        {
            m_resource.release();
            KeLeaveCriticalRegion();
        }

    private:
        EResourceSharedLock(const EResourceSharedLock&);
        EResourceSharedLock& operator=(const EResourceSharedLock&);

    private:
        EResource& m_resource;
    };
}
