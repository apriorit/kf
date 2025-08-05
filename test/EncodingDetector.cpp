#include "pch.h"
#include <kf/EncodingDetector.h>

SCENARIO("EncodingDetector detects BOM encodings")
{
    GIVEN("'EF BB BF T e s t' bytes UTF-8 BOM")
    {
        std::byte data[] = { 
            std::byte(0xEF), std::byte(0xBB), std::byte(0xBF), 
            std::byte('T'), std::byte('e'), std::byte('s'), std::byte('t')
        };
        kf::EncodingDetector detector(data);

        THEN("Detects UTF-8 BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF8);
            REQUIRE(detector.getBomLength() == 3);
        }
    }

    GIVEN("'FF FE T 00 e 00 s 00 t 00' bytes UTF-16LE BOM")
    {
        std::byte data[] = { 
            std::byte(0xFF), std::byte(0xFE),
            std::byte('T'), std::byte(0x00),
            std::byte('e'), std::byte(0x00),
            std::byte('s'), std::byte(0x00),
            std::byte('t'), std::byte(0x00)
        };
        kf::EncodingDetector detector(data);

        THEN("Detects UTF-16LE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16LE);
            REQUIRE(detector.getBomLength() == 2);
        }
    }

    GIVEN("'FE FF 00 T 00 e 00 s 00 t' bytes UTF-16BE BOM")
    {
        std::byte data[] = { 
            std::byte(0xFE), std::byte(0xFF), 
            std::byte(0x00), std::byte('T'),
            std::byte(0x00), std::byte('e'), 
            std::byte(0x00), std::byte('s'),
            std::byte(0x00), std::byte('t')
        };
        kf::EncodingDetector detector(data);

        THEN("Detects UTF-16BE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16BE);
            REQUIRE(detector.getBomLength() == 2);
        }
    }

    GIVEN("'FF FE 00 00 T 00 e 00 s 00 t 00' bytes UTF-32LE BOM")
    {
        std::byte data[] = { 
            std::byte(0xFF), std::byte(0xFE), 
            std::byte(0x00), std::byte(0x00),
            std::byte('T'), std::byte(0x00),
            std::byte('e'), std::byte(0x00),
            std::byte('s'), std::byte(0x00),
            std::byte('t'), std::byte(0x00) 
        };
        kf::EncodingDetector detector(data);

        THEN("Detects UTF-32LE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF32LE);
            REQUIRE(detector.getBomLength() == 4);
        }
    }

    GIVEN("'00 00 FE FF 00 T 00 e 00 s 00 t' bytes UTF-32BE BOM")
    {
        std::byte data[] = {
        std::byte(0x00), std::byte(0x00),
        std::byte(0xFE), std::byte(0xFF),
        std::byte(0x00), std::byte('T'),
        std::byte(0x00), std::byte('e'),
        std::byte(0x00), std::byte('s'),
        std::byte(0x00), std::byte('t')
        };
        kf::EncodingDetector detector(data);

        THEN("Detects UTF-32BE BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF32BE);
            REQUIRE(detector.getBomLength() == 4);
        }
    }
}

SCENARIO("EncodingDetector without BOM")
{
    GIVEN("'T 00 e 00 s 00 t 00' as UTF-16LE without BOM")
    {
        std::byte data[] = {
            std::byte('T'), std::byte(0x00),
            std::byte('e'), std::byte(0x00),
            std::byte('s'), std::byte(0x00),
            std::byte('t'), std::byte(0x00)
        };
        kf::EncodingDetector detector(data);

        THEN("Detects UTF-16LE")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16LE);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("'00 T 00 e 00 s 00 t' as UTF-16BE without BOM")
    {
        std::byte data[] = {
            std::byte(0x00), std::byte('T'),
            std::byte(0x00), std::byte('e'),
            std::byte(0x00), std::byte('s'),
            std::byte(0x00), std::byte('t')
        };
        kf::EncodingDetector detector(data);

        THEN("Detects UTF-16BE without BOM")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::UTF16BE);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("'T e s t' bytes not UTF-16 and without BOM")
    {
        std::byte data[] = {
            std::byte('T'), 
            std::byte('e'), 
            std::byte('s'), 
            std::byte('t')
        };
        kf::EncodingDetector detector(data);

        THEN("Encoding is set to ANSI")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::ANSI);
            REQUIRE(detector.getBomLength() == 0);
        }
    }

    GIVEN("Empty buffer")
    {
        std::byte data[1] = {};
        kf::EncodingDetector detector(data);

        THEN("Encoding is unknown")
        {
            REQUIRE(detector.getEncoding() == kf::EncodingDetector::Unknown);
            REQUIRE(detector.getBomLength() == 0);
        }
    }
}
