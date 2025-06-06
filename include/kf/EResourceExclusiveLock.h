#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // EResourceExclusiveLock

    class [[deprecated("Use std::lock_guard or std::unique_lock instead")]] EResourceExclusiveLock
    {
    public:
        EResourceExclusiveLock(_In_ EResource& resource) : m_resource(resource)
        {
            KeEnterCriticalRegion();
            m_resource.acquireExclusive();
        }

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
