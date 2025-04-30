#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // EResourceExclusiveLock

    class EResourceExclusiveLock
    {
    public:
        EResourceExclusiveLock(_In_ EResource& resource) : m_resource(resource)
        {
            KeEnterCriticalRegion();
            m_resource.acquireExclusive();
        }

        EResourceExclusiveLock(EResourceExclusiveLock&& other) = default;

        ~EResourceExclusiveLock()
        {
            m_resource.release();
            KeLeaveCriticalRegion();
        }

    private:
        EResourceExclusiveLock(const EResourceExclusiveLock&);
        EResourceExclusiveLock& operator=(const EResourceExclusiveLock&);

    private:
        EResource& m_resource;
    };
}
