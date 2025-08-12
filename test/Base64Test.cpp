#include "pch.h"
#include <kf/Base64.h>
#include <string_view>

SCENARIO("Base64::decodeLen")
{
    WHEN("Encoded string is empty")
    {
        THEN("Decode length is 0")
        {
            REQUIRE(kf::Base64::decodeLen(L"") == 0);
        }
    }

    WHEN("Encoded string is VA==")
    {
        THEN("Decode length is 1")
        {
            REQUIRE(kf::Base64::decodeLen(L"VA==") == 1); // "T"
        }
    }

    WHEN("Encoded string is VGU=")
    {
        THEN("Decode length is 2")
        {
            REQUIRE(kf::Base64::decodeLen(L"VGU=") == 2); // "Te"
        }
    }

    WHEN("Encoded string is VGVz")
    {
        THEN("Decode length is 3")
        {
            REQUIRE(kf::Base64::decodeLen(L"VGVz") == 3); // "Tes"
        }
    }

    WHEN("Encoded string is VGVzdA==")
    {
        THEN("Decode length is 4")
        {
            REQUIRE(kf::Base64::decodeLen(L"VGVzdA==") == 4); // "Test"
        }
    }

    WHEN("Encoded string is ==")
    {
        THEN("Decode length is -1")
        {
            REQUIRE(kf::Base64::decodeLen(L"==") == -1);
        }
    }
}


SCENARIO("Base64::decode")
{
    GIVEN("Encoded string 'VA=='")
    {
        kf::USimpleString input(L"VA=="); // "T"
        constexpr std::wstring_view kExpectedOutput = L"T";
        constexpr size_t kExpectedLength = kExpectedOutput.size();
        std::array<wchar_t, kExpectedLength> output;

        THEN("Decoded symbol is 'T' and decoded length is 1")
        {
            int written = kf::Base64::decode(input, std::as_writable_bytes(std::span{ output }));
            REQUIRE(written == kExpectedLength);

            REQUIRE(std::wstring_view(output.data(), written) == kExpectedOutput);
        }
    }

    GIVEN("Encoded string 'VGU='")
    {
        kf::USimpleString input(L"VGU="); // "Te"
        constexpr std::string_view kExpectedOutput = "Te";
        constexpr size_t kExpectedLength = kExpectedOutput.size();
        std::array<char, kExpectedLength> output;

        THEN("Decoded symbols are 'Te' and decoded length is 2")
        {
            int written = kf::Base64::decode(input, std::as_writable_bytes(std::span{ output }));
            REQUIRE(written == kExpectedLength);
            REQUIRE(std::string_view(output.data(), written) == kExpectedOutput);
        }
    }

    GIVEN("Encoded string 'VGhpcyBpcyBhIGxvbmcgdGVzdCBzdHJpbmcgZm9yIHVuaXQgdGVzdGluZw=='")
    {
        kf::USimpleString input(L"VGhpcyBpcyBhIGxvbmcgdGVzdCBzdHJpbmcgZm9yIHVuaXQgdGVzdGluZw==");
        constexpr std::string_view kExpectedOutput = "This is a long test string for unit testing";
        constexpr size_t kExpectedLength = kExpectedOutput.size();
        std::array<char, kExpectedLength> output;

        THEN("Decoded symbols are 'This is a long test string for unit testing' and decoded length is correct")
        {
            int written = kf::Base64::decode(input, std::as_writable_bytes(std::span{ output }));
            REQUIRE(written == kExpectedLength);
            REQUIRE(std::string_view(output.data(), written) == kExpectedOutput);
        }
    }

    GIVEN("Empty string")
    {
        kf::USimpleString input;
        std::array<char, 2> output;

        THEN("Decoded length is 0")
        {
            int written = kf::Base64::decode(input, std::as_writable_bytes(std::span{ output }));
            REQUIRE(written == 0);
        }
    }

    GIVEN("Output buffer isn't big enough")
    {
        kf::USimpleString input(L"VGVzdA==");
        std::array<char, 2> output;

        THEN("Decode returns -1")
        {
            int written = kf::Base64::decode(input, std::as_writable_bytes(std::span{ output }));
            REQUIRE(written == -1);
        }
    }

    GIVEN("Encoded string with negative decode length")
    {
        kf::USimpleString input(L"==");
        std::array<char, 2> output;

        THEN("Decode returns -1")
        {
            int written = kf::Base64::decode(input, std::as_writable_bytes(std::span{ output }));
            REQUIRE(written == -1);
        }
    }
}
