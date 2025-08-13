#include "pch.h"
#include <kf/TextDetector.h>

SCENARIO("TextDetector::isText")
{
    GIVEN("Valid UTF-8 text with BOM")
    {
        constexpr uint8_t kData[] = { 0xEF, 0xBB, 0xBF, 'T', 'e', 's', 't' };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("UTF-8 text with BOM and invalid char")
    {
        constexpr uint8_t kData[] = { 0xEF, 0xBB, 0xBF, 0x01, 'a', 'b' };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("Valid UTF-16LE text with BOM")
    {
        constexpr uint8_t kData[] = {
            0xFF, 0xFE,
            'T', 0x00,
            'e', 0x00,
            's', 0x00,
            't', 0x00
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    }

    GIVEN("UTF-16LE text with BOM and invalid char")
    {
        constexpr uint8_t kData[] = {
            0xFF, 0xFE,
            0x01, 0x00, // invalid char
            'a', 0x00
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("UTF-16BE text with BOM")
    {
        constexpr uint8_t kData[] = {
            0xFE, 0xFF,
            0x00, 'T',
            0x00, 'e',
            0x00, 's',
            0x00, 't'
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("UTF-16BE text with BOM and invalid char")
    {
        constexpr uint8_t kData[] = {
            0xFE, 0xFF,
            0x00, 0x01, // invalid char
            0x00, 'a'
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("Valid UTF-32LE text with BOM")
    {
        constexpr uint8_t kData[] = {
            0xFF, 0xFE, 0x00, 0x00,
            'T', 0x00, 0x00, 0x00,
            'e', 0x00, 0x00, 0x00,
            's', 0x00, 0x00, 0x00,
            't', 0x00, 0x00, 0x00
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    }

    GIVEN("UTF-32LE text with BOM and invalid char")
    {
        constexpr uint8_t kData[] = {
            0xFF, 0xFE, 0x00, 0x00,
            0x01, 0x00, 0x00, 0x00, // invalid char
            'a', 0x00, 0x00, 0x00
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    }

    GIVEN("Valid UTF-32BE text with BOM")
    {
        constexpr uint8_t kData[] = {
            0x00, 0x00, 0xFE, 0xFF,
            0x00, 0x00, 0x00, 'T',
            0x00, 0x00, 0x00, 'e',
            0x00, 0x00, 0x00, 's',
            0x00, 0x00, 0x00, 't'
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    }

    GIVEN("UTF-32BE text with BOM and invalid char")
    {
        constexpr uint8_t kData[] = {
            0x00, 0x00, 0xFE, 0xFF,
            0x00, 0x00, 0x00, 0x01, // invalid char
            0x00, 0x00, 0x00, 'a'
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    }

    GIVEN("Valid UTF-16BE text without BOM")
    {
        constexpr uint8_t kData[] = {
            0x00, 'T',
            0x00, 'e',
            0x00, 's',
            0x00, 't'
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("UTF-16BE text without BOM and invalid char")
    {
        constexpr uint8_t kData[] = {
            0x00, 0x01, // invalid char
            0x00, 'a'
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("Valid UTF-16LE text without BOM")
    {
        constexpr uint8_t kData[] = {
            'T', 0x00,
            'e', 0x00,
            's', 0x00,
            't', 0x00
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    }

    GIVEN("UTF-16LE text without BOM and invalid char")
    {
        constexpr uint8_t kData[] = {
            0x01, 0x00, // invalid char
            'a', 0x00
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("Valid UTF-8 text without BOM")
    {
        constexpr uint8_t kData[] = { 'T', 'e', 's', 't' };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };

    GIVEN("UTF-8 text without BOM and invalid char")
    {
        constexpr uint8_t kData[] = { 0x01, 'a', 'b' };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(std::as_bytes(std::span{ kData })));
        }
    };
}
