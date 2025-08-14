#include "pch.h"
#include <kf/Hex.h>
#include <kf/UString.h>
#include <span>

SCENARIO("Hex::encode")
{
    constexpr UCHAR data[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa,
                    0xb, 0xc, 0xd, 0xe, 0xf, 0x42, 0xfe, 0xd0, 0x5e, 0xf1, 0x40 };
    kf::UString<NonPagedPoolNx> hex;
    auto dataSpan = std::as_bytes(std::span{ data });
    REQUIRE(!kf::Hex::encode(dataSpan, hex));

    const auto encodeLen = kf::Hex::encodeLen(dataSpan);
    REQUIRE(encodeLen == (sizeof(data) * 2));
    REQUIRE_NT_SUCCESS(hex.realloc(encodeLen * sizeof(WCHAR)));

    REQUIRE(kf::Hex::encode(dataSpan, hex));
    REQUIRE(memcmp(hex.buffer(), L"000102030405060708090A0B0C0D0E0F42FED05EF140", hex.byteLength()) == 0);
}

SCENARIO("Hex::decode")
{
    kf::ASimpleString hex("000102030405060708090A0B0C0D0E0F42FED05EF140");
    constexpr int expectedHexLen = 22;
    constexpr UCHAR expectedData[22] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa,
                    0xb, 0xc, 0xd, 0xe, 0xf, 0x42, 0xfe, 0xd0, 0x5e, 0xf1, 0x40 };

    std::array<std::byte, 0> emptyBuffer;
    std::span<std::byte> initialSpan(emptyBuffer.data(), emptyBuffer.size());
    REQUIRE(!kf::Hex::decode(hex, initialSpan));

    const auto decodeLen = kf::Hex::decodeLen(hex);
    REQUIRE(decodeLen == expectedHexLen);
    std::array<std::byte, expectedHexLen> buffer;

    std::span<std::byte> resizedSpan(buffer.data(), buffer.size());
    REQUIRE(kf::Hex::decode(hex, resizedSpan));
    REQUIRE(memcmp(expectedData, resizedSpan.data(), resizedSpan.size()) == 0);
}