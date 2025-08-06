#include "pch.h"
#include <kf/Base64.h>
#include <kf/stl/vector>

SCENARIO("Base64::decodeLen")
{
    WHEN("Encoded string is empty")
    {
        THEN("Decode length is 0")
        {
            REQUIRE(kf::Base64::decodeLen(kf::USimpleString(L"")) == 0);
        }
    }

    WHEN("Encoded string is VA==")
    {
        THEN("Decode length is 1")
        {
            REQUIRE(kf::Base64::decodeLen(kf::USimpleString(L"VA==")) == 1); // "T"
        }
    }

    WHEN("Encoded string is VGU=")
    {
        THEN("Decode length is 2")
        {
            REQUIRE(kf::Base64::decodeLen(kf::USimpleString(L"VGU=")) == 2); // "Te"
        }
    }

    WHEN("Encoded string is VGVz")
    {
        THEN("Decode length is 3")
        {
            REQUIRE(kf::Base64::decodeLen(kf::USimpleString(L"VGVz")) == 3); // "Tes"
        }
    }

    WHEN("Encoded string is VGVzdA==")
    {
        THEN("Decode length is 4")
        {
            REQUIRE(kf::Base64::decodeLen(kf::USimpleString(L"VGVzdA==")) == 4); // "Test"
        }
    }
}


SCENARIO("Base64::decode")
{
    GIVEN("Encoded string 'VA=='")
    {
        kf::USimpleString input(L"VA=="); // "T"
        const char* expectedChars = "T";
        size_t expectedLength = strlen(expectedChars);

        kf::vector<std::byte, NonPagedPoolNx> output;
        REQUIRE_NT_SUCCESS(output.resize(expectedLength));

        THEN("Decoded symbol is 'T' and decoded length is 1")
        {
            int written = kf::Base64::decode(input, std::span<std::byte>(output));
            REQUIRE(written == expectedLength);

            REQUIRE(std::memcmp(output.data(), expectedChars, expectedLength) == 0);
        }
    }

    GIVEN("Encoded string 'TQ=='")
    {
        kf::USimpleString input(L"VGU="); // "Te"
        const char* expectedChars = "Te";
        size_t expectedLength = strlen(expectedChars);

        kf::vector<std::byte, NonPagedPoolNx> output;
        REQUIRE_NT_SUCCESS(output.resize(expectedLength));

        THEN("Decoded symbols are 'Te' and decoded length is 2")
        {
            int written = kf::Base64::decode(input, std::span<std::byte>(output));
            REQUIRE(written == expectedLength);
            REQUIRE(std::memcmp(output.data(), expectedChars, expectedLength) == 0);
        }
    }

    GIVEN("Encoded string 'VGhpcyBpcyBhIGxvbmcgdGVzdCBzdHJpbmcgZm9yIHVuaXQgdGVzdGluZw=='")
    {
        kf::USimpleString input(L"VGhpcyBpcyBhIGxvbmcgdGVzdCBzdHJpbmcgZm9yIHVuaXQgdGVzdGluZw==");
        const char* expectedChars = "This is a long test string for unit testing";
        size_t expectedLength = strlen(expectedChars);

        kf::vector<std::byte, NonPagedPoolNx> output;
        REQUIRE_NT_SUCCESS(output.resize(expectedLength));

        THEN("Decoded symbols are 'This is a long test string for unit testing' and decoded length is correct")
        {
            int written = kf::Base64::decode(input, std::span<std::byte>(output));
            REQUIRE(written == expectedLength);
            REQUIRE(std::memcmp(output.data(), expectedChars, expectedLength) == 0);
        }
    }

    GIVEN("Empty string")
    {
        kf::USimpleString input;
        kf::vector<std::byte, NonPagedPoolNx> output;
        REQUIRE_NT_SUCCESS(output.resize(2));

        THEN("Decoded length is 0")
        {
            int written = kf::Base64::decode(input, std::span<std::byte>(output));
            REQUIRE(written == 0);
        }
    }

    GIVEN("Output buffer isn't big enought")
    {
        kf::USimpleString input(L"VGVzdA==");
        kf::vector<std::byte, NonPagedPoolNx> output;
        REQUIRE_NT_SUCCESS(output.resize(2));

        THEN("Decode returns -1")
        {
            int written = kf::Base64::decode(input, std::span<std::byte>(output));
            REQUIRE(written == -1);
        }
    }
}
