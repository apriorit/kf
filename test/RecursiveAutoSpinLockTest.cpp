#include "pch.h"
#include <kf/RecursiveAutoSpinLock.h>

SCENARIO("kf::RecursiveAutoSpinLock")
{
    GIVEN("RecursiveSpinLock and PASSIVE_LEVEL on entry")
    {
        kf::RecursiveSpinLock lock;

        WHEN("RecursiveAutoSpinLock is created")
        {
            {
                kf::RecursiveAutoSpinLock spinLock(lock);

                THEN("IRQL is DISPATCH_LEVEL while the lock is held")
                {
                    REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);
                }
            }

            THEN("IRQL is restored after the lock is released")
            {
                REQUIRE(KeGetCurrentIrql() == PASSIVE_LEVEL);
            }
        }
    }

    GIVEN("RecursiveAutoSpinLock and pre-raised IRQL to DISPATCH_LEVEL")
    {
        kf::RecursiveSpinLock lock;

        KIRQL oldIrql;
        KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

        WHEN("RecursiveAutoSpinLock is constructed at DISPATCH_LEVEL")
        {
            {
                kf::RecursiveAutoSpinLock spinLock(lock);
                THEN("IRQL remains DISPATCH_LEVEL while held")
                {
                    REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);
                }
            }

            THEN("IRQL stays on the pre-raised DISPATCH_LEVEL")
            {
                REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);
            }
        }

        KeLowerIrql(oldIrql);
    }

    GIVEN("Two RecursiveSpinLocks")
    {
        kf::RecursiveSpinLock lockA;
        kf::RecursiveSpinLock lockB;

        WHEN("Lock A then Lock B are acquired in nested scopes")
        {
            {
                kf::RecursiveAutoSpinLock spinLockA(lockA);
                REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);

                {
                    kf::RecursiveAutoSpinLock spinLockB(lockB);

                    THEN("IRQL inside the inner scope is DISPATCH_LEVEL")
                    {
                        REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);
                    }
                }
            }

            THEN("IRQL is restored after both locks are released")
            {
                REQUIRE(KeGetCurrentIrql() == PASSIVE_LEVEL);
            }
        }
    }

    WHEN("RecursiveAutoSpinLocks are acquiring recursively")
    {
        {
            kf::RecursiveSpinLock lock;
            kf::RecursiveAutoSpinLock spinLock(lock);
            kf::RecursiveAutoSpinLock spinLock2(lock);

            THEN("No deadlock occurs")
            {
                REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);
            }
        }

        THEN("IRQL is restored after both locks are released")
        {
            REQUIRE(KeGetCurrentIrql() == PASSIVE_LEVEL);
        }
    }
}