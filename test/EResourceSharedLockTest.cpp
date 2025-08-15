#include "pch.h"
#include <kf/EResource.h>
#include <kf/EResourceSharedLock.h>

#pragma warning(push)
#pragma warning(disable : 4996) // 'kf::EResourceSharedLock': Use std::shared_lock instead
SCENARIO("kf::EResourceSharedLock:")
{
    GIVEN("EResource object")
    {
        kf::EResource resource;

        WHEN("EResourceSharedLock is created")
        {
            {
                kf::EResourceSharedLock lock(resource);

                THEN("The resource is acquired in shared mode")
                {
                    REQUIRE(resource.isAcquiredExclusive() == false);
                    REQUIRE(resource.isAcquiredShared() > 0);
                }
            }

            THEN("The resource is released when the lock is destroyed")
            {
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }

        WHEN("Multiple EResourceSharedLock objects are created")
        {
            {
                kf::EResourceSharedLock lock1(resource);
                kf::EResourceSharedLock lock2(resource);

                THEN("The resource is acquired in shared mode by multiple locks")
                {
                    REQUIRE(resource.isAcquiredExclusive() == false);
                    REQUIRE(resource.isAcquiredShared() == 2);
                }
            }

            THEN("The resource is released when all locks are destroyed")
            {
                REQUIRE(resource.isAcquiredExclusive() == false);
                REQUIRE(resource.isAcquiredShared() == 0);
            }
        }
    }
}
#pragma warning(pop)