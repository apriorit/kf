#include "pch.h"
#include <kf/TextDetector.h>

SCENARIO("TextDetector::isText")
{
    GIVEN("Valid UTF-8 text with BOM")
    {
        std::byte data[] = {
        std::byte(0xEF), std::byte(0xBB), std::byte(0xBF),
        std::byte('T'), std::byte('e'), std::byte('s'), std::byte('t')
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(data));
        }
    };

    GIVEN("UTF-8 text with BOM and invalid char")
    {
        std::byte data[] = {
            std::byte(0xEF), std::byte(0xBB), std::byte(0xBF),
            std::byte(0x01), // invalid char
            std::byte('a'), std::byte('b')
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(data));
        }
    };

    GIVEN("Valid UTF-16LE text with BOM")
    {
        std::byte data[] = {
            std::byte(0xFF), std::byte(0xFE),
            std::byte('T'), std::byte(0x00),
            std::byte('e'), std::byte(0x00),
            std::byte('s'), std::byte(0x00),
            std::byte('t'), std::byte(0x00)
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(data));
        }
    }

    GIVEN("UTF-16LE text with BOM and invalid char")
    {
        std::byte data[] = {
            std::byte(0xFF), std::byte(0xFE),
            std::byte(0x01), std::byte(0x00), // invalid char
            std::byte('a'), std::byte(0x00)
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(data));
        }
    };

    GIVEN("UTF-16BE text with BOM")
    {
        std::byte data[] = {
            std::byte(0xFE), std::byte(0xFF),
            std::byte(0x00), std::byte('T'),
            std::byte(0x00), std::byte('e'),
            std::byte(0x00), std::byte('s'),
            std::byte(0x00), std::byte('t')
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(data));
        }
    };

    GIVEN("UTF-16BE text with BOM and invalid char")
    {
        std::byte data[] = {
        std::byte(0xFE), std::byte(0xFF),
        std::byte(0x00), std::byte(0x01), // invalid char
        std::byte(0x00), std::byte('a')
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(data));
        }
    };

    GIVEN("Valid UTF-32LE text with BOM")
    {
        std::byte data[] = {
            std::byte(0xFF), std::byte(0xFE), std::byte(0x00), std::byte(0x00),
            std::byte('T'), std::byte(0x00), std::byte(0x00), std::byte(0x00),
            std::byte('e'), std::byte(0x00), std::byte(0x00), std::byte(0x00)
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(data));
        }
    }

    GIVEN("UTF-32LE text with BOM and invalid char")
    {
        std::byte data[] = {
            std::byte(0xFF), std::byte(0xFE), std::byte(0x00), std::byte(0x00),
            std::byte(0x01), std::byte(0x00), std::byte(0x00), std::byte(0x00), // invalid char
            std::byte('a'), std::byte(0x00), std::byte(0x00), std::byte(0x00)
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(data));
        }
    }

    GIVEN("Valid UTF-32BE text with BOM")
    {
        std::byte data[] = {
            std::byte(0x00), std::byte(0x00), std::byte(0xFE), std::byte(0xFF),
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte('T'),
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte('e')
        };

        THEN("Text is detected")
        {
            REQUIRE(kf::TextDetector::isText(data));
        }
    }

    GIVEN("UTF-32BE text with BOM and invalid char")
    {
        std::byte data[] = {
            std::byte(0x00), std::byte(0x00), std::byte(0xFE), std::byte(0xFF),
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte(0x01), // invalid char
            std::byte(0x00), std::byte(0x00), std::byte(0x00), std::byte('a')
        };

        THEN("Not a text is detected")
        {
            REQUIRE(!kf::TextDetector::isText(data));
        }
    }
}
