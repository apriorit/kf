#include "pch.h"
#include <kf/Hex.h>
#include <kf/UString.h>
#include <span>
#include <array>

SCENARIO("Hex::encode")
{
    GIVEN("A byte array of data")
    {
        constexpr UCHAR kData[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa,
                                    0xb, 0xc, 0xd, 0xe, 0xf, 0x42, 0xfe, 0xd0, 0x5e, 0xf1, 0x40 };
        const auto dataSpan = std::as_bytes(std::span{ kData });
        kf::UString<NonPagedPoolNx> hexString;

        WHEN("encodeLen of buffer")
        {
            THEN("kf::Hex::encodeLen returns correct length")
            {
                constexpr size_t kExpectedEncodeLen = sizeof(kData) * 2;
                const auto encodeLen = kf::Hex::encodeLen(dataSpan);
                REQUIRE(encodeLen == kExpectedEncodeLen);
            }
        }

        WHEN("Encoding into UString with insufficient buffer")
        {
            THEN("kf::Hex::encode return false")
            {
                REQUIRE(!kf::Hex::encode(dataSpan, hexString));
            }
        }

        WHEN("Encoding into UString were buffer size is equal to encodeLen")
        {
            const auto encodeLen = kf::Hex::encodeLen(dataSpan);
            REQUIRE_NT_SUCCESS(hexString.realloc(encodeLen * sizeof(WCHAR)));

            THEN("kf::Hex::encode return true and correct UString")
            {
                constexpr WCHAR kExpectedHexString[] = L"000102030405060708090A0B0C0D0E0F42FED05EF140";
                REQUIRE(kf::Hex::encode(dataSpan, hexString));
                REQUIRE(memcmp(hexString.buffer(), kExpectedHexString, hexString.byteLength()) == 0);
            }
        }
    }
}

SCENARIO("Hex::decode")
{
    GIVEN("An encoded ASimpleString")
    {
        const kf::ASimpleString hexString("000102030405060708090A0B0C0D0E0F42FED05EF140");
        constexpr UCHAR kExpectedData[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa,
                                            0xb, 0xc, 0xd, 0xe, 0xf, 0x42, 0xfe, 0xd0, 0x5e, 0xf1, 0x40 };
        constexpr int kExpectedDataLen = ARRAYSIZE(kExpectedData);

        WHEN("encodeLen of buffer")
        {
            THEN("kf::Hex::decodeLen returns correct length")
            {
                const auto decodeLen = kf::Hex::decodeLen(hexString);
                REQUIRE(decodeLen == kExpectedDataLen);
            }
        }

        WHEN("Decoding into an empty buffer")
        {
            std::array<std::byte, 0> buffer;
            std::span<std::byte> bufferSpan = buffer;

            THEN("kf::Hex::decode returns false")
            {
                REQUIRE(!kf::Hex::decode(hexString, bufferSpan));
            }
        }

        WHEN("Decoding into a buffer with insufficient size")
        {
            std::array<std::byte, kExpectedDataLen - 1> buffer;
            std::span<std::byte> bufferSpan = buffer;

            THEN("kf::Hex::decode returns false")
            {
                REQUIRE(!kf::Hex::decode(hexString, bufferSpan));
            }
        }

        WHEN("Decoding into a buffer with size equal to decodeLen")
        {
            std::array<std::byte, kExpectedDataLen> buffer;
            std::span<std::byte> bufferSpan = buffer;

            THEN("kf::Hex::decode returns true and correct data")
            {
                REQUIRE(kf::Hex::decode(hexString, bufferSpan));
                REQUIRE(memcmp(kExpectedData, bufferSpan.data(), bufferSpan.size()) == 0);
            }
        }
    }

    GIVEN("ASimpleString with invalid characters")
    {
        const kf::ASimpleString invalidHex("ZZZZ");

        THEN("kf::Hex::decode returns false")
        {
            std::array<std::byte, 4> buffer;
            std::span<std::byte> bufferSpan = buffer;

            REQUIRE(!kf::Hex::decode(invalidHex, bufferSpan));
        }
    }
}