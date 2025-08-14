#include "pch.h"
#include <kf/AutoSpinLock.h>

SCENARIO("kf::AutoSpinLock")
{
    GIVEN("An initialized KSPIN_LOCK and PASSIVE_LEVEL on entry")
    {
        KSPIN_LOCK lock;
        KeInitializeSpinLock(&lock);

        KIRQL irqlBefore = KeGetCurrentIrql();

        WHEN("AutoSpinLock enters a scope")
        {
            {
                kf::AutoSpinLock spinLock(&lock);

                THEN("IRQL is DISPATCH_LEVEL while the lock is held")
                {
                    REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);
                }
            }

            THEN("IRQL is restored after the lock is released")
            {
                REQUIRE(KeGetCurrentIrql() == irqlBefore);
            }
        }
    }

    GIVEN("An initialized KSPIN_LOCK and pre-raised IRQL to DISPATCH_LEVEL")
    {
        KSPIN_LOCK lock;
        KeInitializeSpinLock(&lock);

        KIRQL oldIrql;
        KeRaiseIrql(DISPATCH_LEVEL, &oldIrql);

        WHEN("AutoSpinLock is constructed at DISPATCH_LEVEL")
        {
            {
                kf::AutoSpinLock guard(&lock);
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

    GIVEN("Two initialized KSPIN_LOCK objects")
    {
        KSPIN_LOCK lockA, lockB;
        KeInitializeSpinLock(&lockA);
        KeInitializeSpinLock(&lockB);
        KIRQL irqlBefore = KeGetCurrentIrql();

        WHEN("Lock A then Lock B are acquired in nested scopes")
        {
            {
                kf::AutoSpinLock guardA(&lockA);
                REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);

                {
                    kf::AutoSpinLock guardB(&lockB);

                    THEN("IRQL inside the inner scope is DISPATCH_LEVEL")
                    {
                        REQUIRE(KeGetCurrentIrql() == DISPATCH_LEVEL);
                    }
                }
            }

            THEN("IRQL is restored after both locks are released")
            {
                REQUIRE(KeGetCurrentIrql() == irqlBefore);
            }
        }
    }
}
