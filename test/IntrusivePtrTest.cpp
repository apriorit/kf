#include "pch.h"
#include <kf/stl/new>
#include <kf/boost/intrusive_ptr.hpp>
#include <kf/boost/intrusive_ref_counter.hpp>

struct IntrusivePtrTestStruct : public boost::intrusive_ref_counter<IntrusivePtrTestStruct>
{
    virtual ~IntrusivePtrTestStruct() = default;

    UINT64 number = 0;
};

using IntrusivePtrTestStructPtr = boost::intrusive_ptr<IntrusivePtrTestStruct>;

// TODO: implement benchmark that tests the usage of the same object in several threads

SCENARIO("intrusive_ptr: methods")
{
    GIVEN("two IntrusivePtrTestStruct are initialized")
    {
        IntrusivePtrTestStructPtr struct1(new(PagedPool) IntrusivePtrTestStruct);
        struct1->number = 42;

        IntrusivePtrTestStructPtr struct2(new(PagedPool) IntrusivePtrTestStruct);
        struct2->number = 533;

        WHEN("operator bool is called")
        {
            THEN("it returns true for non-empty object")
            {
                REQUIRE(struct1);
                REQUIRE(struct2);
            }
        }

        WHEN("get is called")
        {
            auto ptr1 = struct1.get();
            auto ptr2 = struct2.get();

            THEN("pointers are not nullptr")
            {
                REQUIRE(ptr1 != nullptr);
                REQUIRE(ptr2 != nullptr);
            }

            THEN("pointers are different")
            {
                REQUIRE(ptr1 != ptr2);
            }
        }

        WHEN("detach is called")
        {
            auto ptr = struct1.detach();

            THEN("the object doesn't belong to the smart pointer")
            {
                REQUIRE(struct1.get() == nullptr);
            }

            THEN("detached object is accessible and valid")
            {
                REQUIRE(ptr != nullptr);
                REQUIRE(ptr->number == 42);
            }

            delete ptr;
        }

        WHEN("reset is called on the same object")
        {
            struct1.reset(struct2.detach());

            THEN("the holder of the donor is nullptr")
            {
                REQUIRE(struct2.get() == nullptr);
            }

            THEN("the holder of recipient holds the donor")
            {
                REQUIRE(struct1.get() != nullptr);
                REQUIRE(struct1->number == 533);
            }
        }

        WHEN("swap is called")
        {
            struct1.swap(struct2);

            THEN("the content is swapped")
            {
                REQUIRE(struct1->number == 533);
                REQUIRE(struct2->number == 42);
            }
        }

        WHEN("operator= is called")
        {
            struct1 = struct2.detach();

            THEN("the holder of recipient holds the donor")
            {
                REQUIRE(struct1.get() != nullptr);
                REQUIRE(struct1->number == 533);
            }
        }

        WHEN("operator* is called")
        {
            auto& ref1 = *struct1;
            auto& ref2 = *struct2;

            THEN("it dereferences the proper object")
            {
                REQUIRE(ref1.number == 42);
                REQUIRE(ref2.number == 533);
            }
        }
    }
}

SCENARIO("intrusive_ptr: reference counter")
{
    GIVEN("an IntrusivePtrTestStruct object")
    {
        IntrusivePtrTestStructPtr structPtr(new(PagedPool) IntrusivePtrTestStruct);
        structPtr->number = 42;

        WHEN("use_count is called")
        {
            THEN("it returns 1 for the first holder")
            {
                REQUIRE(structPtr->use_count() == 1);
                REQUIRE(structPtr.get() != nullptr);
                REQUIRE(structPtr->number == 42);
            }
        }

        WHEN("the holder is assigned to nullptr")
        {
            structPtr = nullptr;
            THEN("use_count becomes 0 and the object is deleted")
            {
                REQUIRE(structPtr.get() == nullptr);
            }
        }

        WHEN("another holder is created")
        {
            IntrusivePtrTestStructPtr anotherHolder = structPtr;
            THEN("use_count increases to 2")
            {
                REQUIRE(structPtr->use_count() == 2);
                REQUIRE(anotherHolder->use_count() == 2);
                REQUIRE(structPtr.get() != nullptr);
                REQUIRE(anotherHolder.get() != nullptr);
                REQUIRE(structPtr->number == 42);
                REQUIRE(anotherHolder->number == 42);
            }

            WHEN("the second holder is reset")
            {
                anotherHolder.reset();
                THEN("use_count decreases to 1")
                {
                    REQUIRE(structPtr->use_count() == 1);
                }
            }
        }

        WHEN("the object is moved")
        {
            IntrusivePtrTestStructPtr movedHolder = std::move(structPtr);
            THEN("use_count remains 1 for the moved holder")
            {
                REQUIRE(movedHolder->use_count() == 1);
                REQUIRE(structPtr.get() == nullptr);
                REQUIRE(movedHolder.get() != nullptr);
                REQUIRE(movedHolder->number == 42);
            }
        }
    }
}

struct IntrusivePtrDtorCheck : public IntrusivePtrTestStruct
{
    static inline int dtorCallCount = 0;

    ~IntrusivePtrDtorCheck()
    {
        ++dtorCallCount;
    }
};

using IntrusivePtrDtorCheckPtr = boost::intrusive_ptr<IntrusivePtrDtorCheck>;

SCENARIO("intrusive_ptr: destructor is called only once")
{
    GIVEN("an IntrusivePtrDtorCheck object")
    {
        IntrusivePtrDtorCheck::dtorCallCount = 0;

        {
            IntrusivePtrDtorCheckPtr ptr1(new(PagedPool) IntrusivePtrDtorCheck);
            REQUIRE(ptr1.get() != nullptr);
            REQUIRE(IntrusivePtrDtorCheck::dtorCallCount == 0);

            {
                IntrusivePtrDtorCheckPtr ptr2 = ptr1;
                REQUIRE(ptr1->use_count() == 2);
                REQUIRE(IntrusivePtrDtorCheck::dtorCallCount == 0);
            }

            REQUIRE(ptr1->use_count() == 1);
            REQUIRE(IntrusivePtrDtorCheck::dtorCallCount == 0);
        }

        THEN("destructor is called exactly once after all references are gone")
        {
            REQUIRE(IntrusivePtrDtorCheck::dtorCallCount == 1);
        }
    }
}
