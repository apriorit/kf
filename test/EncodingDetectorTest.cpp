#include "pch.h"
#include <kf/EncodingDetector.h>

SCENARIO("EncodingDetector detects BOM encodings")
{
    GIVEN("'EF BB BF T e s t' bytes UTF-8 BOM")
    {
        constexpr uint8_t kData[] = {
            0xEF, 0xBB, 0xBF,
            'T', 'e', 's', 't'
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Detects UTF-8 BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF8);
            REQUIRE(detector.getBomLength() == 3);
        }
    }

    GIVEN("'FF FE T 00 e 00 s 00 t 00' bytes UTF-16LE BOM")
    {
        constexpr uint8_t kData[] = {
            0xFF, 0xFE,
            'T', 0x00,
            'e', 0x00,
            's', 0x00,
            't', 0x00
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Detects UTF-16LE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16LE);
            REQUIRE(detector.getBomLength() == 2);
        }
    }

    GIVEN("'FE FF 00 T 00 e 00 s 00 t' bytes UTF-16BE BOM")
    {
        constexpr uint8_t kData[] = {
            0xFE, 0xFF,
            0x00, 'T',
            0x00, 'e',
            0x00, 's',
            0x00, 't'
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Detects UTF-16BE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16BE);
            REQUIRE(detector.getBomLength() == 2);
        }
    }

    GIVEN("'FF FE 00 00 T 00 00 00 e 00 00 00 s 00 00 00 t 00 00 00' bytes UTF-32LE BOM")
    {
        constexpr uint8_t kData[] = {
            0xFF, 0xFE, 0x00, 0x00,
            'T',  0x00, 0x00,  0x00,
            'e',  0x00, 0x00,  0x00,
            's',  0x00, 0x00,  0x00,
            't',  0x00, 0x00,  0x00
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Detects UTF-32LE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF32LE);
            REQUIRE(detector.getBomLength() == 4);
        }
    }

    GIVEN("'00 00 FE FF 00 00 00 T 00 00 00 e 00 00 00 s 00 00 00 t' bytes UTF-32BE BOM")
    {
        constexpr uint8_t kData[] = {
            0x00, 0x00, 0xFE, 0xFF,
            0x00, 0x00, 0x00, 'T',
            0x00, 0x00, 0x00, 'e',
            0x00, 0x00, 0x00, 's',
            0x00, 0x00, 0x00, 't'
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Detects UTF-32BE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF32BE);
            REQUIRE(detector.getBomLength() == 4);
        }
    }
}

SCENARIO("EncodingDetector without BOM")
{
    GIVEN("'T e s t' ANSI bytes and without BOM")
    {
        constexpr uint8_t kData[] = { 'T', 'e', 's', 't' };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Encoding is set to ANSI")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::ANSI);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("'T 00 e 00 s 00 t 00' as UTF-16LE without BOM")
    {
        constexpr uint8_t kData[] = {
            'T', 0x00,
            'e', 0x00,
            's', 0x00,
            't', 0x00
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Detects UTF-16LE")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16LE);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("'00 T 00 e 00 s 00 t' as UTF-16BE without BOM")
    {
        constexpr uint8_t kData[] = {
            0x00, 'T',
            0x00, 'e',
            0x00, 's',
            0x00, 't'
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Detects UTF-16BE without BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16BE);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("Small buffer with zeros only (<kMaximumBomLength)")
    {
        constexpr uint8_t kData[1] = {};
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Encoding is unknown")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::Unknown);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("Big buffer with zeros only (>kMaximumBomLength)")
    {
        constexpr uint8_t kData[10] = {};
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Encoding is ANSI")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::ANSI);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("'00 T 00 e 00 s 00 t 00 00' bytes without BOM and with random position of 0s and odd size")
    {
        constexpr uint8_t kData[] = {
            0x00, 'T',
            'e', 0x00,
            's', 0x00,
            't', 0x00,
            0x00
        };
        kf::EncodingDetector detector(std::as_bytes(std::span{ kData }));

        THEN("Encoding is ANSI")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::ANSI);
            REQUIRE(detector.getBomLength() == 0);
        }
    }
}
