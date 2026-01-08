#include "pch.h"
#include <kf/UStringBuilder.h>
#include <kf/UString.h>

using namespace kf;

SCENARIO("UStringBuilder")
{
    GIVEN("A default constructed builder")
    {
        UStringBuilder<PagedPool> sb;

        THEN("It holds an empty string")
        {
            REQUIRE(sb.string().charLength() == 0);
            REQUIRE(sb.string().maxCharLength() == 0);
        }

        WHEN("reserve(0) is called")
        {
            NTSTATUS status = sb.reserve(0);

            THEN("It succeeds and remains empty")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().charLength() == 0);
                REQUIRE(sb.string().maxCharLength() == 0);
            }
        }

        WHEN("reserve(10) is called")
        {
            NTSTATUS status = sb.reserve(10);

            THEN("Capacity grows and length stays 0")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().charLength() == 0);
                REQUIRE(sb.string().maxCharLength() == 10);
            }
        }
    }

    GIVEN("An empty builder")
    {
        UStringBuilder<PagedPool> sb;

        WHEN("A single PCWSTR is appended")
        {
            NTSTATUS status = sb.append(L"abc");

            THEN("The resulting string equals 'abc'")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().equals(L"abc"));
            }
        }

        WHEN("A single USimpleString is appended")
        {
            NTSTATUS status = sb.append(USimpleString(L"def"));
            THEN("The resulting string equals 'def'")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().equals(L"def"));
            }
        }

        WHEN("A single UNICODE_STRING is appended")
        {
            UNICODE_STRING us = RTL_CONSTANT_STRING(L"ghi");
            NTSTATUS status = sb.append(us);
            THEN("The resulting string equals 'ghi'")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().equals(L"ghi"));
            }
        }

        WHEN("A single UString is appended")
        {
            UString<PagedPool> us;
            REQUIRE_NT_SUCCESS(us.init(L"xyz"));

            NTSTATUS status = sb.append(us);
            THEN("The resulting string equals 'xyz'")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().equals(L"xyz"));
            }
        }

        WHEN("A single span<const wchar_t> is appended")
        {
            const wchar_t data[] = { L'm', L'n', L'o' };
            NTSTATUS status = sb.append(span{ data });

            THEN("The resulting string equals 'mno'")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().equals(L"mno"));
            }
        }

        WHEN("A single span<const std::byte> is appended")
        {
            // UTF-16LE bytes for "uvw": u=0x0075, v=0x0076, w=0x0077
            const std::byte bytes[] = 
            {
                std::byte{0x75}, std::byte{0x00},
                std::byte{0x76}, std::byte{0x00},
                std::byte{0x77}, std::byte{0x00}
            };
            NTSTATUS status = sb.append(span { bytes });

            THEN("The resulting string equals 'uvw'")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().equals(L"uvw"));
            }
        }

        WHEN("USimpleString and PCWSTR are appended in sequence")
        {
            NTSTATUS status1 = sb.append(L"ab");
            NTSTATUS status2 = sb.append(USimpleString(L"cd"));
            NTSTATUS status3 = sb.append(L"");

            THEN("They concatenate to 'abcd'")
            {
                REQUIRE_NT_SUCCESS(status1);
                REQUIRE_NT_SUCCESS(status2);
                REQUIRE_NT_SUCCESS(status3);
                REQUIRE(sb.string().equals(L"abcd"));
            }
        }

        WHEN("Variadic append with multiple arguments is used")
        {
            NTSTATUS status = sb.append(USimpleString(L"ab"), USimpleString(L"cd"), L"ef");

            THEN("All parts are appended in order")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().equals(L"abcdef"));
            }
        }
    }

    GIVEN("A builder with content 'Hello'")
    {
        UStringBuilder<PagedPool> sb;
        REQUIRE_NT_SUCCESS(sb.append(L"Hello"));

        WHEN("reserve is called with a smaller size than current length")
        {
            // Note: current charLength is 5, reserve(3) will reallocate to 3 and truncate
            NTSTATUS status = sb.reserve(3);

            THEN("It truncates the current content")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().charLength() == 3);
                REQUIRE(sb.string().maxCharLength() == 3);
                REQUIRE(sb.string().equals(L"Hel"));
            }
        }
    }
}

