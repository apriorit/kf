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
            REQUIRE(sb.string().isEmpty());
            REQUIRE(sb.string().charLength() == 0);
            REQUIRE(sb.string().maxCharLength() == 0);
        }

        WHEN("reserve(0) is called")
        {
            NTSTATUS status = sb.reserve(0);

            THEN("It succeeds and remains empty")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().isEmpty());
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
                REQUIRE(sb.string().charLength() == 3);
                REQUIRE(sb.string().equals(L"abc"));
            }
        }

        WHEN("USimpleString and PCWSTR are appended in sequence")
        {
            NTSTATUS status1 = sb.append(L"ab");
            USimpleString part(L"cd");
            NTSTATUS status2 = sb.append(part);
            NTSTATUS status3 = sb.append(L"");

            THEN("They concatenate to 'abcd'")
            {
                REQUIRE_NT_SUCCESS(status1);
                REQUIRE_NT_SUCCESS(status2);
                REQUIRE_NT_SUCCESS(status3);
                REQUIRE(sb.string().charLength() == 4);
                REQUIRE(sb.string().equals(L"abcd"));
            }
        }

        WHEN("Variadic append with multiple arguments is used")
        {
            USimpleString a(L"ab");
            USimpleString c(L"cd");
            NTSTATUS status = sb.append(a, c, L"ef");

            THEN("All parts are appended in order")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().charLength() == 6);
                REQUIRE(sb.string().equals(L"abcdef"));
            }
        }

        WHEN("Multiple append calls trigger internal reallocation and preserve content")
        {
            REQUIRE_NT_SUCCESS(sb.append(L"abc"));
            // Force growth by appending a longer chunk
            REQUIRE_NT_SUCCESS(sb.append(L"defghij"));

            THEN("Final content is the concatenation of all parts")
            {
                REQUIRE(sb.string().charLength() == 10);
                REQUIRE(sb.string().equals(L"abcdefghij"));
            }
        }
    }

    GIVEN("A builder with content 'Hello'")
    {
        UStringBuilder<PagedPool> sb;
        REQUIRE_NT_SUCCESS(sb.append(L"Hello"));

        WHEN("copyTo is used with a small destination buffer")
        {
            WCHAR buf[4] = { 0 };
            int copied = sb.string().copyTo(buf);
            THEN("It copies as many chars as fits including null-terminator in destination")
            {
                REQUIRE(copied == 3); // min(4-1, 5) = 3
                REQUIRE(buf[0] == L'H');
                REQUIRE(buf[1] == L'e');
                REQUIRE(buf[2] == L'l');
                REQUIRE(buf[3] == L'\0');
            }
        }

        WHEN("reserve is called with a smaller size than current length")
        {
            // Note: current charLength is 5, reserve(3) will reallocate to 3 and truncate
            NTSTATUS status = sb.reserve(3);

            THEN("It truncates the current content")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(sb.string().charLength() == 3);
                REQUIRE(sb.string().equals(L"Hel"));
                REQUIRE(sb.string().maxCharLength() == 3);
            }
        }
    }
}

