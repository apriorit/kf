#include "pch.h"
#include <kf/Guard.h>

namespace
{
    struct GuardResource
    {
        unsigned int closeCount = 0;
        int value = 42;
    };

    void closeResource(GuardResource* resource)
    {
        ++resource->closeCount;
    }

    void closeResourceAsVoid(void* resource)
    {
        closeResource(static_cast<GuardResource*>(resource));
    }

    using TestGuard = GUARD_TYPE(GuardResource*, nullptr, &closeResource);
    using ConvertedGuard = kf::Guard::Guard<GuardResource*, nullptr, decltype(&closeResourceAsVoid), &closeResourceAsVoid, void*>;
    constexpr ULONG_PTR kInvalidValue = static_cast<ULONG_PTR>(-1);
    using IntegerGuard = kf::Guard::Guard<ULONG_PTR, kInvalidValue, decltype(&closeResource), &closeResource, GuardResource*>;

    static_assert(!std::is_copy_constructible<TestGuard>::value, "Guards must not be copy constructed");
    static_assert(!std::is_copy_assignable<TestGuard>::value, "Guards must not be copy assigned");
    static_assert(std::is_move_constructible<TestGuard>::value, "Guards must support move construction");
    static_assert(std::is_move_assignable<TestGuard>::value, "Guards must support move assignment");
    static_assert(!std::is_convertible<GuardResource*, TestGuard>::value, "Construction must be explicit");
    static_assert(std::is_same<decltype(std::declval<TestGuard&>().get()), GuardResource*&>::value, "Mutable get must return a reference");
    static_assert(std::is_same<decltype(std::declval<const TestGuard&>().get()), GuardResource*>::value, "Const get must return the value");
}

SCENARIO("kf::Guard construction")
{
    GuardResource resource;

    GIVEN("No initial value")
    {
        WHEN("A guard is default constructed")
        {
            TestGuard guard;

            THEN("It contains the invalid value")
            {
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("The invalid value")
    {
        WHEN("A guard is explicitly constructed with it")
        {
            TestGuard guard(nullptr);

            THEN("It contains the invalid value")
            {
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("A valid resource")
    {
        WHEN("A guard is explicitly constructed with it")
        {
            TestGuard guard(&resource);

            THEN("It owns the resource without closing it")
            {
                REQUIRE(guard.get() == &resource);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }
}

SCENARIO("kf::Guard access")
{
    GuardResource resource;

    GIVEN("An empty guard")
    {
        TestGuard guard;
        const TestGuard& constGuard = guard;

        WHEN("Mutable get is called")
        {
            GuardResource*& value = guard.get();

            THEN("It returns a reference to the invalid value")
            {
                REQUIRE(value == nullptr);
                REQUIRE(&value == &guard.get());
            }
        }

        WHEN("Const get is called")
        {
            const auto value = constGuard.get();

            THEN("It returns the invalid value")
            {
                REQUIRE(value == nullptr);
            }
        }

        WHEN("The guard is implicitly converted to its value type")
        {
            GuardResource* value = constGuard;

            THEN("The converted value is invalid")
            {
                REQUIRE(value == nullptr);
            }
        }

        WHEN("The arrow operator is called")
        {
            const auto value = constGuard.operator->();

            THEN("It returns the invalid value")
            {
                REQUIRE(value == nullptr);
            }
        }

        WHEN("A resource is assigned through mutable get")
        {
            guard.get() = &resource;

            THEN("The guard holds the assigned resource without closing it")
            {
                REQUIRE(guard.get() == &resource);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }

    GIVEN("A guard owning a resource")
    {
        TestGuard guard(&resource);
        const TestGuard& constGuard = guard;

        WHEN("Mutable get is called")
        {
            GuardResource*& value = guard.get();

            THEN("It returns a reference to the stored resource pointer")
            {
                REQUIRE(value == &resource);
                REQUIRE(&value == &guard.get());
                REQUIRE(resource.closeCount == 0);
            }
        }

        WHEN("Const get is called")
        {
            const auto value = constGuard.get();

            THEN("It returns the resource without closing it")
            {
                REQUIRE(value == &resource);
                REQUIRE(resource.closeCount == 0);
            }
        }

        WHEN("The guard is implicitly converted to its value type")
        {
            GuardResource* value = constGuard;

            THEN("The converted value is the resource pointer")
            {
                REQUIRE(value == &resource);
                REQUIRE(resource.closeCount == 0);
            }
        }

        WHEN("The arrow operator is called")
        {
            const auto value = constGuard.operator->();

            THEN("It returns the resource pointer")
            {
                REQUIRE(value == &resource);
                REQUIRE(resource.closeCount == 0);
            }
        }

        WHEN("A member is read through the const arrow operator")
        {
            const auto value = constGuard->value;

            THEN("It returns the resource member value")
            {
                REQUIRE(value == 42);
            }
        }

        WHEN("A member is modified through the arrow operator")
        {
            guard->value = 7;

            THEN("The resource member is updated")
            {
                REQUIRE(resource.value == 7);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }
}

SCENARIO("kf::Guard reset")
{
    GuardResource first;
    GuardResource second;

    GIVEN("An empty guard")
    {
        TestGuard guard;

        WHEN("It is reset with no argument")
        {
            guard.reset();

            THEN("It remains empty")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(first.closeCount == 0);
            }
        }

        WHEN("It is reset to the explicit invalid value")
        {
            guard.reset(nullptr);

            THEN("It remains empty")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(first.closeCount == 0);
            }
        }

        WHEN("It is reset to a valid resource")
        {
            guard.reset(&first);

            THEN("It acquires the resource without closing it")
            {
                REQUIRE(guard.get() == &first);
                REQUIRE(first.closeCount == 0);
            }
        }
    }

    GIVEN("A guard owning a resource")
    {
        TestGuard guard(&first);

        WHEN("It is reset with no argument")
        {
            guard.reset();

            THEN("It becomes empty and closes the resource once")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(first.closeCount == 1);
            }
        }

        WHEN("It is reset to the explicit invalid value")
        {
            guard.reset(nullptr);

            THEN("It becomes empty and closes the resource once")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(first.closeCount == 1);
            }
        }

        WHEN("It is reset to a different resource")
        {
            guard.reset(&second);

            THEN("It closes the old resource and owns the new one")
            {
                REQUIRE(guard.get() == &second);
                REQUIRE(first.closeCount == 1);
                REQUIRE(second.closeCount == 0);
            }
        }

        WHEN("It is reset to the same resource")
        {
            guard.reset(&first);

            THEN("It closes the old value and retains the same pointer")
            {
                REQUIRE(guard.get() == &first);
                REQUIRE(first.closeCount == 1);
            }
        }
    }

    GIVEN("A guard already reset to empty")
    {
        TestGuard guard(&first);
        guard.reset();

        WHEN("It is reset again")
        {
            guard.reset();

            THEN("The original resource is not closed again")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(first.closeCount == 1);
            }
        }

        WHEN("It is reset to the explicit invalid value")
        {
            guard.reset(nullptr);

            THEN("The original resource is not closed again")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(first.closeCount == 1);
            }
        }
    }

    GIVEN("A guard that has released its resource")
    {
        TestGuard guard(&first);
        guard.release();

        WHEN("It is reset to another resource")
        {
            guard.reset(&second);

            THEN("It owns the new resource without closing either resource")
            {
                REQUIRE(guard.get() == &second);
                REQUIRE(first.closeCount == 0);
                REQUIRE(second.closeCount == 0);
            }
        }

        WHEN("It is reset with no argument")
        {
            guard.reset();

            THEN("The released resource is not closed")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(first.closeCount == 0);
            }
        }
    }
}

SCENARIO("kf::Guard release")
{
    GuardResource resource;

    GIVEN("An empty guard")
    {
        TestGuard guard;

        WHEN("Ownership is released")
        {
            const auto value = guard.release();

            THEN("It returns the invalid value and remains empty")
            {
                REQUIRE(value == nullptr);
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("A guard owning a resource")
    {
        TestGuard guard(&resource);

        WHEN("Ownership is released")
        {
            const auto value = guard.release();

            THEN("It returns the resource and becomes empty without closing it")
            {
                REQUIRE(value == &resource);
                REQUIRE(guard.get() == nullptr);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }

    GIVEN("A guard that has already released its resource")
    {
        TestGuard guard(&resource);
        guard.release();

        WHEN("Ownership is released again")
        {
            const auto value = guard.release();

            THEN("It returns the invalid value without closing the resource")
            {
                REQUIRE(value == nullptr);
                REQUIRE(guard.get() == nullptr);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }
}

SCENARIO("kf::Guard move construction")
{
    GuardResource resource;
    GuardResource reusedResource;

    GIVEN("An empty source")
    {
        TestGuard source;

        WHEN("A destination is move constructed from it")
        {
            TestGuard destination(std::move(source));

            THEN("Both guards remain empty")
            {
                REQUIRE(source.get() == nullptr);
                REQUIRE(destination.get() == nullptr);
            }
        }
    }

    GIVEN("An owning source")
    {
        TestGuard source(&resource);

        WHEN("A destination is move constructed from it")
        {
            TestGuard destination(std::move(source));

            THEN("Ownership transfers without closing the resource")
            {
                REQUIRE(source.get() == nullptr);
                REQUIRE(destination.get() == &resource);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }

    GIVEN("A source whose ownership has been moved")
    {
        TestGuard source(&resource);
        TestGuard destination(std::move(source));

        WHEN("The source is reset to another resource")
        {
            source.reset(&reusedResource);

            THEN("Both guards independently own their resources")
            {
                REQUIRE(source.get() == &reusedResource);
                REQUIRE(destination.get() == &resource);
                REQUIRE(resource.closeCount == 0);
                REQUIRE(reusedResource.closeCount == 0);
            }
        }
    }
}

SCENARIO("kf::Guard move assignment")
{
    GuardResource sourceResource;
    GuardResource destinationResource;

    GIVEN("An empty source and an empty destination")
    {
        TestGuard source;
        TestGuard destination;

        WHEN("The source is move assigned to the destination")
        {
            TestGuard& result = (destination = std::move(source));

            THEN("Both guards remain empty and assignment returns the destination")
            {
                REQUIRE(&result == &destination);
                REQUIRE(source.get() == nullptr);
                REQUIRE(destination.get() == nullptr);
            }
        }
    }

    GIVEN("An owning source and an empty destination")
    {
        TestGuard source(&sourceResource);
        TestGuard destination;

        WHEN("The source is move assigned to the destination")
        {
            TestGuard& result = (destination = std::move(source));

            THEN("Ownership transfers without closing the source resource")
            {
                REQUIRE(&result == &destination);
                REQUIRE(source.get() == nullptr);
                REQUIRE(destination.get() == &sourceResource);
                REQUIRE(sourceResource.closeCount == 0);
            }
        }
    }

    GIVEN("An empty source and an owning destination")
    {
        TestGuard source;
        TestGuard destination(&destinationResource);

        WHEN("The source is move assigned to the destination")
        {
            TestGuard& result = (destination = std::move(source));

            THEN("The destination resource is closed and both guards become empty")
            {
                REQUIRE(&result == &destination);
                REQUIRE(source.get() == nullptr);
                REQUIRE(destination.get() == nullptr);
                REQUIRE(destinationResource.closeCount == 1);
            }
        }
    }

    GIVEN("An owning source and an owning destination")
    {
        TestGuard source(&sourceResource);
        TestGuard destination(&destinationResource);

        WHEN("The source is move assigned to the destination")
        {
            TestGuard& result = (destination = std::move(source));

            THEN("Only the old destination resource is closed")
            {
                REQUIRE(&result == &destination);
                REQUIRE(source.get() == nullptr);
                REQUIRE(destination.get() == &sourceResource);
                REQUIRE(sourceResource.closeCount == 0);
                REQUIRE(destinationResource.closeCount == 1);
            }
        }
    }

    GIVEN("An owning guard and an alias to itself")
    {
        TestGuard guard(&sourceResource);
        TestGuard& alias = guard;

        WHEN("Self-move assignment is performed")
        {
            TestGuard& result = (guard = std::move(alias));

            THEN("Ownership is unchanged and assignment returns itself")
            {
                REQUIRE(&result == &guard);
                REQUIRE(guard.get() == &sourceResource);
                REQUIRE(sourceResource.closeCount == 0);
            }
        }
    }

    GIVEN("An empty guard and an alias to itself")
    {
        TestGuard guard;
        TestGuard& alias = guard;

        WHEN("Self-move assignment is performed")
        {
            TestGuard& result = (guard = std::move(alias));

            THEN("The guard remains empty and assignment returns itself")
            {
                REQUIRE(&result == &guard);
                REQUIRE(guard.get() == nullptr);
            }
        }
    }

    GIVEN("Ownership already transferred from the first guard to the second")
    {
        TestGuard first(&sourceResource);
        TestGuard second;
        TestGuard third;
        second = std::move(first);

        WHEN("The second guard is move assigned to the third")
        {
            TestGuard& result = (third = std::move(second));

            THEN("Only the third guard owns the resource")
            {
                REQUIRE(&result == &third);
                REQUIRE(first.get() == nullptr);
                REQUIRE(second.get() == nullptr);
                REQUIRE(third.get() == &sourceResource);
                REQUIRE(sourceResource.closeCount == 0);
            }
        }

        WHEN("The first guard is reset to another resource")
        {
            first.reset(&destinationResource);

            THEN("The first and second guards independently own their resources")
            {
                REQUIRE(first.get() == &destinationResource);
                REQUIRE(second.get() == &sourceResource);
                REQUIRE(sourceResource.closeCount == 0);
                REQUIRE(destinationResource.closeCount == 0);
            }
        }
    }
}

SCENARIO("kf::Guard destruction")
{
    GuardResource first;
    GuardResource second;

    GIVEN("A default constructed guard")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard;
            }

            THEN("No cleanup of an invalid value is attempted")
            {
                REQUIRE(first.closeCount == 0);
            }
        }
    }

    GIVEN("A guard explicitly constructed with the invalid value")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(nullptr);
            }

            THEN("No cleanup of an invalid value is attempted")
            {
                REQUIRE(first.closeCount == 0);
            }
        }
    }

    GIVEN("A guard owning a resource")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(&first);
            }

            THEN("The resource is closed exactly once")
            {
                REQUIRE(first.closeCount == 1);
            }
        }
    }

    GIVEN("A guard assigned a resource through mutable get")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard;
                guard.get() = &first;
            }

            THEN("The assigned resource is closed exactly once")
            {
                REQUIRE(first.closeCount == 1);
            }
        }
    }

    GIVEN("A guard already reset to empty")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(&first);
                guard.reset();
            }

            THEN("The original resource is not closed again")
            {
                REQUIRE(first.closeCount == 1);
            }
        }
    }

    GIVEN("A guard already reset to a different resource")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(&first);
                guard.reset(&second);
            }

            THEN("The replacement is closed and the original is not closed again")
            {
                REQUIRE(first.closeCount == 1);
                REQUIRE(second.closeCount == 1);
            }
        }
    }

    GIVEN("A guard already reset to the same resource")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(&first);
                guard.reset(&first);
            }

            THEN("The retained value is closed a second time")
            {
                REQUIRE(first.closeCount == 2);
            }
        }
    }

    GIVEN("A guard that has released its resource")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(&first);
                guard.release();
            }

            THEN("The released resource is not closed")
            {
                REQUIRE(first.closeCount == 0);
            }
        }
    }

    GIVEN("A guard reused after release")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(&first);
                guard.release();
                guard.reset(&second);
            }

            THEN("Only the newly acquired resource is closed")
            {
                REQUIRE(first.closeCount == 0);
                REQUIRE(second.closeCount == 1);
            }
        }
    }

    GIVEN("Ownership transferred by move construction")
    {
        TestGuard source(&first);

        WHEN("The destination lifetime ends")
        {
            {
                TestGuard destination(std::move(source));
            }

            THEN("The transferred resource is closed exactly once")
            {
                REQUIRE(source.get() == nullptr);
                REQUIRE(first.closeCount == 1);
            }
        }
    }

    GIVEN("Ownership transferred by move assignment")
    {
        WHEN("The destination lifetime ends")
        {
            TestGuard source(&first);
            {
                TestGuard destination(&second);
                destination = std::move(source);
            }

            THEN("Each resource is closed exactly once")
            {
                REQUIRE(source.get() == nullptr);
                REQUIRE(first.closeCount == 1);
                REQUIRE(second.closeCount == 1);
            }
        }

        WHEN("The moved-from source lifetime ends")
        {
            TestGuard destination(&second);
            {
                TestGuard source(&first);
                destination = std::move(source);
            }

            THEN("The transferred resource is not closed")
            {
                REQUIRE(first.closeCount == 0);
                REQUIRE(second.closeCount == 1);
                REQUIRE(destination.get() == &first);
            }
        }
    }

    GIVEN("A guard that has been self-move assigned")
    {
        WHEN("Its lifetime ends")
        {
            {
                TestGuard guard(&first);
                TestGuard& alias = guard;
                guard = std::move(alias);
            }

            THEN("Its resource is closed exactly once")
            {
                REQUIRE(first.closeCount == 1);
            }
        }
    }

    GIVEN("A moved-from guard reused with another resource")
    {
        WHEN("The reused source lifetime ends")
        {
            TestGuard destination;
            {
                TestGuard source(&first);
                destination = std::move(source);
                source.reset(&second);
            }

            THEN("Only the newly acquired resource is closed")
            {
                REQUIRE(first.closeCount == 0);
                REQUIRE(second.closeCount == 1);
                REQUIRE(destination.get() == &first);
            }
        }

        WHEN("The destination lifetime ends")
        {
            TestGuard source(&first);
            {
                TestGuard destination(std::move(source));
                source.reset(&second);
            }

            THEN("Only the transferred resource is closed")
            {
                REQUIRE(first.closeCount == 1);
                REQUIRE(second.closeCount == 0);
                REQUIRE(source.get() == &second);
            }
        }
    }
}

SCENARIO("kf::Guard custom value and cleanup types")
{
    GuardResource resource;
    const auto value = reinterpret_cast<ULONG_PTR>(&resource);

    GIVEN("A cleanup function accepting a different pointer type")
    {
        ConvertedGuard guard(&resource);

        WHEN("The guard is reset")
        {
            guard.reset();

            THEN("The resource is converted to the cleanup type and closed once")
            {
                REQUIRE(guard.get() == nullptr);
                REQUIRE(resource.closeCount == 1);
            }
        }
    }

    GIVEN("An owning guard with a different cleanup pointer type")
    {
        WHEN("Its lifetime ends")
        {
            {
                ConvertedGuard guard(&resource);
            }

            THEN("The converted resource is closed exactly once")
            {
                REQUIRE(resource.closeCount == 1);
            }
        }
    }

    GIVEN("An integer guard with a nonzero invalid sentinel")
    {
        WHEN("It is default constructed")
        {
            IntegerGuard guard;

            THEN("It contains the custom invalid sentinel")
            {
                REQUIRE(guard.get() == kInvalidValue);
            }
        }
    }

    GIVEN("An empty integer guard")
    {
        IntegerGuard guard;

        WHEN("It is reset with no argument")
        {
            guard.reset();

            THEN("It retains the custom invalid sentinel without cleanup")
            {
                REQUIRE(guard.get() == kInvalidValue);
                REQUIRE(resource.closeCount == 0);
            }
        }

        WHEN("Ownership is released")
        {
            const auto released = guard.release();

            THEN("It returns the custom invalid sentinel")
            {
                REQUIRE(released == kInvalidValue);
                REQUIRE(guard.get() == kInvalidValue);
                REQUIRE(resource.closeCount == 0);
            }
        }

        WHEN("It is reset to an integer resource value")
        {
            guard.reset(value);

            THEN("It owns that value without closing the resource")
            {
                REQUIRE(guard.get() == value);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }

    GIVEN("An integer guard owning a resource")
    {
        IntegerGuard guard(value);

        WHEN("It is reset with no argument")
        {
            guard.reset();

            THEN("It closes the converted resource and restores the custom sentinel")
            {
                REQUIRE(guard.get() == kInvalidValue);
                REQUIRE(resource.closeCount == 1);
            }
        }

        WHEN("Ownership is released")
        {
            const auto released = guard.release();

            THEN("It returns the integer value and restores the custom sentinel")
            {
                REQUIRE(released == value);
                REQUIRE(guard.get() == kInvalidValue);
                REQUIRE(resource.closeCount == 0);
            }
        }

        WHEN("A destination is move constructed from it")
        {
            IntegerGuard destination(std::move(guard));

            THEN("The source receives the custom sentinel and ownership transfers")
            {
                REQUIRE(guard.get() == kInvalidValue);
                REQUIRE(destination.get() == value);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }

    GIVEN("An integer guard already reset to empty")
    {
        WHEN("Its lifetime ends")
        {
            {
                IntegerGuard guard(value);
                guard.reset();
            }

            THEN("The resource is not closed a second time")
            {
                REQUIRE(resource.closeCount == 1);
            }
        }
    }

    GIVEN("An integer guard that has released its resource")
    {
        IntegerGuard guard(value);
        guard.release();

        WHEN("It is reset to the resource value again")
        {
            guard.reset(value);

            THEN("It reacquires the resource without closing it")
            {
                REQUIRE(guard.get() == value);
                REQUIRE(resource.closeCount == 0);
            }
        }
    }
}
