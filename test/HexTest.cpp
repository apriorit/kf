#include "pch.h"
#include <kf/Hex.h>
#include <kf/UString.h>
#include <kf/ScopedBuffer.h>

// TODO: implement ASCII string and test encode/decode for ASimpleString

SCENARIO("Hex test: encode to Unicode")
{
    unsigned char data[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa,
                    0xb, 0xc, 0xd, 0xe, 0xf, 0x42, 0xfe, 0xd0, 0x5e, 0xf1, 0x40 };
    kf::UString<NonPagedPoolNx> hex;

    const std::span<std::byte> dataSpan(reinterpret_cast<std::byte*>(data), sizeof(data));

    REQUIRE(false == kf::Hex::encode(dataSpan, hex));
    const auto encodeLen = kf::Hex::encodeLen(dataSpan);
    REQUIRE(encodeLen == (sizeof(data) * sizeof(WCHAR)));
    REQUIRE_NT_SUCCESS(hex.realloc(encodeLen * sizeof(WCHAR)));

    REQUIRE(true == kf::Hex::encode(dataSpan, hex));
    REQUIRE(0 == wcscmp(hex.buffer(), L"000102030405060708090A0B0C0D0E0F42FED05EF140"));
}

SCENARIO("Hex test: decode from Unicode")
{
    kf::USimpleString hex(L"000102030405060708090A0B0C0D0E0F42FED05EF140");
    unsigned char expectedData[] = { 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xa,
                    0xb, 0xc, 0xd, 0xe, 0xf, 0x42, 0xfe, 0xd0, 0x5e, 0xf1, 0x40 };

    kf::scoped_buffer<std::byte, NonPagedPoolNx> data;
    std::span<std::byte> emptySpan(data.get(), data.size());
    REQUIRE(false == kf::Hex::decode(hex, emptySpan));

    const auto decodeLen = kf::Hex::decodeLen(hex);
    REQUIRE(decodeLen == sizeof(expectedData));
    REQUIRE_NT_SUCCESS(data.resize(decodeLen));

    std::span<std::byte> dataSpan(data.get(), data.size());
    REQUIRE(true == kf::Hex::decode(hex, dataSpan));
    REQUIRE(0 == memcmp(expectedData, data.get(), data.size()));
}