#include "pch.h"
#include <kf/UString.h>

// The following test tests are only for the UString class, which is an owning string
// class derived from USimpleString.
// These tests rely on the USimpleString class to provide basic string functionality,
// so we don't need to test those functionalities here.

SCENARIO("UString: allocation and deallocation")
{
    GIVEN("An empty UString")
    {
        kf::UString<POOL_TYPE::NonPagedPool> ustr;

        WHEN("it is initialized with a Unicode string")
        {
            NTSTATUS status = ustr.init(L"Hello, World!");

            THEN("it should succeed")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"Hello, World!")); // 13 characters
            }
        }

        WHEN("it is initialized with an ANSI string")
        {
            ANSI_STRING ansiString;
            RtlInitAnsiString(&ansiString, "Hello, ANSI World!");
            NTSTATUS status = ustr.init(ansiString);

            THEN("it should succeed and convert to Unicode")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"Hello, ANSI World!"));
            }

            THEN("it should have the correct byte length")
            {
                REQUIRE(ustr.byteLength() == 36); // 18 characters * 2 bytes each
            }

            THEN("it should have the correct max byte length")
            {
                REQUIRE(ustr.maxByteLength() == 38); // 18 characters * 2 bytes + 2 bytes for null terminator
            }
        }

        WHEN("it is initialized with a USimpleString")
        {
            kf::USimpleString simpleStr(L"Static String");
            NTSTATUS status = ustr.init(simpleStr);

            THEN("it should succeed and copy the content")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"Static String"));
            }

            THEN("it should have ownership on the copy of the original string")
            {
                REQUIRE(ustr.string().Buffer != simpleStr.string().Buffer);
            }
        }

        WHEN("it is initialized with a Unicode string and byte length smaller than actual length")
        {
            NTSTATUS status = ustr.init(L"Hello, World!", 10);

            THEN("it should succeed and truncate the string")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"Hello")); // 5 characters * 2 bytes each
            }
        }

        WHEN("it is initialized with a null string")
        {
            NTSTATUS status = ustr.init(nullptr);

            THEN("it should succeed and be empty")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.isEmpty());
            }
        }

        WHEN("it is initialized with an empty string")
        {
            NTSTATUS status = ustr.init(L"");

            THEN("it should succeed and be empty")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.isEmpty());
            }
        }

        WHEN("it is reallocated to a larger size")
        {
            REQUIRE_NT_SUCCESS(ustr.init(L"Hello, World!"));
            NTSTATUS status = ustr.realloc(64);

            THEN("it should succeed and keep the original content")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"Hello, World!"));
            }

            THEN("it should have proper byteLength")
            {
                REQUIRE(ustr.byteLength() == 26);
            }

            THEN("it should have new maxByteLength")
            {
                REQUIRE(ustr.maxByteLength() == 64);
            }
        }

        WHEN("it is reallocated to a smaller size")
        {
            REQUIRE_NT_SUCCESS(ustr.init(L"Hello, World!"));
            NTSTATUS status = ustr.realloc(10);

            THEN("it should succeed and truncate the string")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"Hello"));
            }

            THEN("it should have proper byteLength")
            {
                REQUIRE(ustr.byteLength() == 10); // 5 characters * 2 bytes each
            }

            THEN("it should have new maxByteLength")
            {
                REQUIRE(ustr.maxByteLength() == 10); // 5 characters * 2 bytes each
            }
        }

        WHEN("it is freed")
        {
            ustr.free();
            THEN("it should be empty")
            {
                REQUIRE(ustr.isEmpty());
                REQUIRE(ustr.byteLength() == 0);
            }
        }
    }
}

SCENARIO("UString: movement and assigning")
{
    GIVEN("A string with initial content")
    {
        kf::UString<POOL_TYPE::PagedPool> ustr;
        REQUIRE_NT_SUCCESS(ustr.init(L"Initial String"));
        
        WHEN("it is moved to another UString")
        {
            kf::UString<POOL_TYPE::PagedPool> movedUstr(std::move(ustr));

            THEN("the original string should be empty")
            {
                REQUIRE(ustr.isEmpty());
            }

            THEN("the moved string should have the original content")
            {
                REQUIRE(movedUstr.equals(L"Initial String"));
            }
        }

        WHEN("it is assigned to another UString")
        {
            kf::UString<POOL_TYPE::PagedPool> assignedUstr;
            assignedUstr = std::move(ustr);

            THEN("the original string should be empty")
            {
                REQUIRE(ustr.isEmpty());
            }

            THEN("the assigned string should have the original content")
            {
                REQUIRE(assignedUstr.equals(L"Initial String"));
            }
        }
    }
}

SCENARIO("Ustring: all methods")
{
    GIVEN("A UString with initial content")
    {
        kf::UString<POOL_TYPE::PagedPool> ustr;
        REQUIRE_NT_SUCCESS(ustr.init(L"abc"));

        WHEN("concat is called without reallocation for appended string")
        {
            NTSTATUS status = ustr.concat(L"ghi");

            THEN("it should return STATUS_BUFFER_TOO_SMALL")
            {
                REQUIRE(status == STATUS_BUFFER_TOO_SMALL);
            }

            THEN("the original string should remain unchanged")
            {
                REQUIRE(ustr.equals(L"abc"));
            }
        }

        WHEN("concat is called with reallocation for appended string")
        {
            REQUIRE_NT_SUCCESS(ustr.realloc(12)); // 6 characters * 2 bytes each
            NTSTATUS status = ustr.concat(L"ghi");

            THEN("it should append the string")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"abcghi"));
            }
        }

        WHEN("format is called without reallocation for the formatted string")
        {
            NTSTATUS status = ustr.format(L"Value: %d", 42);

            THEN("it should return STATUS_BUFFER_OVERFLOW")
            {
                REQUIRE(status == STATUS_BUFFER_OVERFLOW);
            }
        }

        WHEN("format is called with reallocation for the formatted string")
        {
            REQUIRE_NT_SUCCESS(ustr.realloc(18)); // 9 characters * 2 bytes each
            NTSTATUS status = ustr.format(L"Value: %d", 42);

            THEN("it should format the string properly")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"Value: 42"));
            }
        }

        WHEN("toUpperCase is called")
        {
            NTSTATUS status = ustr.toUpperCase();

            THEN("it should convert the string to upper case")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"ABC"));
            }
        }

        WHEN("toLowerCase is called")
        { 
            REQUIRE_NT_SUCCESS(ustr.init(L"XYZ"));
            NTSTATUS status = ustr.toLowerCase();

            THEN("it should convert the string to lower case")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.equals(L"xyz"));
            }
        }
    }

    GIVEN("An empty UString")
    {
        kf::UString<POOL_TYPE::NonPagedPool> ustr;

        WHEN("toUpperCase is called")
        {
            NTSTATUS status = ustr.toUpperCase();

            THEN("it should succeed and remain empty")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.isEmpty());
            }
        }

        WHEN("toLowerCase is called")
        {
            NTSTATUS status = ustr.toLowerCase();

            THEN("it should succeed and remain empty")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(ustr.isEmpty());
            }
        }
    }
}