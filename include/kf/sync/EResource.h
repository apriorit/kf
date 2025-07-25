#pragma once

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // EResource

    class EResource
    {
    public:
        EResource();
        ~EResource();

        operator PERESOURCE();

        bool acquireExclusive(_In_ _Literal_ bool wait = true);
        bool acquireShared(_In_ bool wait = true);
        bool acquireSharedStarveExclusive(_In_ bool wait = true);
        bool acquireSharedWaitForExclusive(_In_ bool wait = true);

        _Requires_lock_held_(m_resource)
        void convertExclusiveToShared();

        _Requires_lock_held_(m_resource)
        void release();

        _Requires_lock_held_(m_resource)
        void releaseForThread(ERESOURCE_THREAD resourceThreadId);

        bool isAcquiredExclusive();
        ULONG isAcquiredShared();

        ULONG getExclusiveWaiterCount();
        ULONG getSharedWaiterCount();
        ULONG getOwnerCount();

        void fltAcquireExclusive();
        void fltAcquireShared();
        void fltRelease();

        // Lockable
        void lock();
        void unlock();
        bool try_lock() = delete;

        // SharedLockable
        void lock_shared();
        void unlock_shared();
        bool try_lock_shared() = delete;

    private:
        EResource(const EResource&);
        EResource& operator=(const EResource&);

    private:
        ERESOURCE m_resource;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // EResource - inline

    inline EResource::EResource()
    {
        ::ExInitializeResourceLite(&m_resource);
    }

    inline EResource::~EResource()
    {
        ::ExDeleteResourceLite(&m_resource);
    }

    inline EResource::operator PERESOURCE()
    {
        return &m_resource;
    }

    inline bool EResource::acquireExclusive(_In_ _Literal_ bool wait)
    {
        return !!::ExAcquireResourceExclusiveLite(&m_resource, wait);
    }

    inline bool EResource::acquireShared(_In_ bool wait)
    {
        return !!::ExAcquireResourceSharedLite(&m_resource, wait);
    }

    inline bool EResource::acquireSharedStarveExclusive(_In_ bool wait)
    {
        return !!::ExAcquireSharedStarveExclusive(&m_resource, wait);
    }

    inline bool EResource::acquireSharedWaitForExclusive(_In_ bool wait)
    {
        return !!::ExAcquireSharedWaitForExclusive(&m_resource, wait);
    }

    _Requires_lock_held_(m_resource)
    inline void EResource::convertExclusiveToShared()
    {
        ::ExConvertExclusiveToSharedLite(&m_resource);
    }

    _Requires_lock_held_(m_resource)
    inline void EResource::release()
    {
        ::ExReleaseResourceLite(&m_resource);
    }

    _Requires_lock_held_(m_resource)
    inline void EResource::releaseForThread(ERESOURCE_THREAD resourceThreadId)
    {
        ::ExReleaseResourceForThreadLite(&m_resource, resourceThreadId);
    }

    inline bool EResource::isAcquiredExclusive()
    {
        return !!::ExIsResourceAcquiredExclusiveLite(&m_resource);
    }

    inline ULONG EResource::isAcquiredShared()
    {
        return ::ExIsResourceAcquiredSharedLite(&m_resource);
    }

    inline ULONG EResource::getExclusiveWaiterCount()
    {
        return ::ExGetExclusiveWaiterCount(&m_resource);
    }

    inline ULONG EResource::getSharedWaiterCount()
    {
        return ::ExGetSharedWaiterCount(&m_resource);
    }

    inline ULONG EResource::getOwnerCount()
    {
        return isAcquiredShared();
    }

    inline void EResource::lock()
    {
        KeEnterCriticalRegion();
        acquireExclusive();
    }

    _Requires_lock_held_(m_resource)
    inline void EResource::unlock()
    {
        release();
        KeLeaveCriticalRegion();
    }

    inline void EResource::lock_shared()
    {
        KeEnterCriticalRegion();
        acquireShared();
    }

    _Requires_lock_held_(m_resource)
    inline void EResource::unlock_shared()
    {
        release();
        KeLeaveCriticalRegion();
    }
}
