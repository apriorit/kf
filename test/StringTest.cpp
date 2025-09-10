#include "pch.h"
#include <kf/stl/string>

SCENARIO("string to numeric conversion")
{
    GIVEN("string with positive decimal")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("42"));

        constexpr int expectedResult = 42;
        constexpr size_t expectedIndex = 2;
        int result = 0;
        size_t index = 0;

        WHEN("stoi with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index);

            THEN("parse success and index points after number")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string with negative decimal")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("-77"));

        constexpr int expectedResult = -77;
        constexpr size_t expectedIndex = 3;
        int result = 0;
        size_t index = 0;

        WHEN("stoi with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index);

            THEN("parse negative success and index points after number")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string with negative decimal and non-numeric tail")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("-77tail"));

        constexpr int expectedResult = -77;
        constexpr size_t expectedIndex = 3;
        int result = 0;
        size_t index = 0;

        WHEN("stoi with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index);

            THEN("parse negative success and index points at tail's begining")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string with octal number")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("075"));

        constexpr int expectedResult = 61;
        constexpr size_t expectedIndex = 3;
        int result = 0;
        size_t index = 0;

        WHEN("stoi base 8 with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index, 8);

            THEN("parse octal success and index is correct")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string with hexadecimal number")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("0x2A"));

        constexpr int expectedResult = 42;
        constexpr size_t expectedIndex = 4;
        int result = 0;
        size_t index = 0;

        WHEN("stoi base 16 with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index, 16);

            THEN("parse hex success and index is correct")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string with hexadecimal number and non-hex char")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("0x2Az"));

        constexpr int expectedResult = 42;
        constexpr size_t expectedIndex = 4;
        int result = 0;
        size_t index = 0;

        WHEN("stoi base 16 with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index, 16);

            THEN("parse hex success and index correct")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string with base 5 number")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("132"));

        constexpr int expectedResult = 42; // 1*25 + 3*5 + 2
        constexpr size_t expectedIndex = 3;
        int result = 0;
        size_t index = 0;

        WHEN("stoi base 5 with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index, 5);

            THEN("parse base 5 success and index is correct")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string with base 5 number and non-base 5 char")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("1328"));

        constexpr int expectedResult = 42; // 1*25 + 3*5 + 2
        constexpr size_t expectedIndex = 3;
        int result = 0;
        size_t index = 0;

        WHEN("stoi base 5 with index is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index, 5);

            THEN("parse base 5 success and index is correct")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("empty string")
    {
        kf::string<PagedPool> s;

        int result = 0;
        size_t index = 0;

        WHEN("stoi is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index);

            THEN("returns invalid parameter")
            {
                REQUIRE(status == STATUS_INVALID_PARAMETER);
            }
        }
    }

    GIVEN("string with invalid characters")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("abc"));

        int result = 0;
        size_t index = 0;

        WHEN("stoi is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index);

            THEN("returns invalid parameter")
            {
                REQUIRE(status == STATUS_INVALID_PARAMETER);
            }
        }
    }

    GIVEN("string causing overflow for int")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("999999999999"));

        int result = 0;
        size_t index = 0;

        WHEN("stoi is called")
        {
            NTSTATUS status = kf::stoi(s, result, &index);

            THEN("returns integer overflow")
            {
                REQUIRE(status == STATUS_INTEGER_OVERFLOW);
            }
        }
    }

    GIVEN("string positive for unsigned long")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("123abc"));

        constexpr unsigned long expectedResult = 123UL;
        constexpr size_t expectedIndex = 3;
        unsigned long result = 0;
        size_t index = 0;

        WHEN("stoul with index is called")
        {
            NTSTATUS status = kf::stoul(s, result, &index);

            THEN("parse success and index is correct")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string negative for unsigned long")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("-42"));

        unsigned long result = 0;
        size_t index = 0;

        WHEN("stoul is called")
        {
            NTSTATUS status = kf::stoul(s, result, &index);

            THEN("returns integer overflow")
            {
                REQUIRE(status == STATUS_INTEGER_OVERFLOW);
            }
        }
    }

    GIVEN("string with min long long")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("-9223372036854775808"));

        constexpr long long expectedResult = (std::numeric_limits<long long>::min)();
        constexpr size_t expectedIndex = 20;
        long long result = 0;
        size_t index = 0;

        WHEN("stoll is called")
        {
            NTSTATUS status = kf::stoll(s, result, &index);

            THEN("parse min value success")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string which overflows long long")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("9223372036854775808"));

        long long result = 0;
        size_t index = 0;

        WHEN("stoll is called")
        {
            NTSTATUS status = kf::stoll(s, result, &index);

            THEN("returns integer overflow")
            {
                REQUIRE(status == STATUS_INTEGER_OVERFLOW);
            }
        }
    }

    GIVEN("string overflow unsigned long long")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("18446744073709551615")); // unsigned long long max

        constexpr unsigned long long expectedResult = (std::numeric_limits<unsigned long long>::max)();
        constexpr size_t expectedIndex = 20;
        unsigned long long result = 0;
        size_t index = 0;

        WHEN("stoull is called")
        {
            NTSTATUS status = kf::stoull(s, result, &index);

            THEN("parse value success")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("string overflow unsigned long long")
    {
        kf::string<PagedPool> s;
        REQUIRE_NT_SUCCESS(s.assign("18446744073709551616")); // unsigned long long max + 1

        unsigned long long result = 0;
        size_t index = 0;

        WHEN("stoull is called")
        {
            NTSTATUS status = kf::stoull(s, result, &index);

            THEN("returns integer overflow")
            {
                REQUIRE(status == STATUS_INTEGER_OVERFLOW);
            }
        }
    }

    GIVEN("wstring with decimal positive")
    {
        kf::wstring<PagedPool> ws;
        REQUIRE_NT_SUCCESS(ws.assign(L"42"));

        constexpr long long expectedResult = 42;
        constexpr size_t expectedIndex = 2;
        int result = 0;
        size_t index = 0;

        WHEN("stoi is called")
        {
            NTSTATUS status = kf::stoi(ws, result, &index);

            THEN("parse success and index points after number")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("wstring with hexadecimal negative")
    {
        kf::wstring<PagedPool> ws;
        REQUIRE_NT_SUCCESS(ws.assign(L"-0xFF"));

        constexpr long long expectedResult = -255LL;
        constexpr size_t expectedIndex =5;
        long long result = 0;
        size_t index = 0;

        WHEN("stoll with base 0 is called")
        {
            NTSTATUS status = kf::stoll(ws, result, &index, 0);

            THEN("parse success and index points after number")
            {
                REQUIRE(status == STATUS_SUCCESS);
                REQUIRE(result == expectedResult);
                REQUIRE(index == expectedIndex);
            }
        }
    }

    GIVEN("wstring with invalid chars")
    {
        kf::wstring<PagedPool> ws;
        REQUIRE_NT_SUCCESS(ws.assign(L"abc"));

        unsigned long long result = 0;
        size_t index = 0;


        WHEN("stoull is called")
        {
            NTSTATUS status = kf::stoull(ws, result, &index);

            THEN("return invalid parameter, result and index = 0")
            {
                REQUIRE(status == STATUS_INVALID_PARAMETER);
                REQUIRE(result == 0);
                REQUIRE(index == 0);
            }
        }
    }
}

SCENARIO("to_string for numeric values")
{
    kf::string<PagedPool> buffer;

    GIVEN("positive int value")
    {
        constexpr int value = 42;
        constexpr const char* expectedStr = "42";

        WHEN("to_string is called")
        {
            NTSTATUS status = to_string(value, buffer);

            THEN("conversion succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(buffer == expectedStr);
            }
        }
    }

    GIVEN("negative int value")
    {
        constexpr int value = -77;
        constexpr const char* expectedStr = "-77";

        WHEN("to_string is called")
        {
            NTSTATUS status = to_string(value, buffer);

            THEN("conversion succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(buffer == expectedStr);
            }
        }
    }

    GIVEN("unsigned long long value and string buffer")
    {
        constexpr unsigned long long value = 1234567890123456789ULL;
        constexpr const char* expectedStr = "1234567890123456789";

        WHEN("to_string is called")
        {
            NTSTATUS status = to_string(value, buffer);

            THEN("conversion succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(buffer == expectedStr);
            }
        }
    }
}

SCENARIO("to_wstring for numeric values")
{
    kf::wstring<PagedPool> buffer;

    GIVEN("positive int value")
    {
        constexpr int value = 123;
        constexpr const wchar_t* expectedStr = L"123";

        WHEN("to_wstring is called")
        {
            NTSTATUS status = to_wstring(value, buffer);

            THEN("conversion succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(buffer == expectedStr);
            }
        }
    }

    GIVEN("negative int and wstring buffer")
    {
        constexpr int value = -55;
        constexpr const wchar_t* expectedStr = L"-55";

        WHEN("to_wstring is called")
        {
            NTSTATUS status = to_wstring(value, buffer);

            THEN("conversion succeeds")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(buffer == expectedStr);
            }
        }
    }

    GIVEN("long long value and wstring buffer")
    {
        constexpr long long value = 9223372036854775807LL; // LLONG_MAX
        constexpr const wchar_t* expectedStr = L"9223372036854775807";

        WHEN("to_wstring is called")
        {
            NTSTATUS status = kf::to_wstring(value, buffer);

            THEN("conversion succeeds and buffer contains string representation")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(buffer == expectedStr);
            }
        }
    }

    GIVEN("unsigned long long value and wstring buffer")
    {
        constexpr unsigned long long value = 18446744073709551615ULL; // ULLONG_MAX
        constexpr const wchar_t* expectedStr = L"18446744073709551615";

        WHEN("to_wstring is called")
        {
            NTSTATUS status = kf::to_wstring(value, buffer);

            THEN("conversion succeeds and buffer contains string representation")
            {
                REQUIRE_NT_SUCCESS(status);
                REQUIRE(buffer == expectedStr);
            }
        }
    } 
}
