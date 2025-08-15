#pragma once
#include <span>
#include <algorithm>
#include <intrin.h>
#include "EncodingDetector.h"
#include "SpanUtils.h"

namespace kf
{
    using namespace std;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // TextDetector class provides a utility to determine whether a given buffer contains textual data.
    // It filters out control characters (except for \t, \n, \r) to verify that the content represents valid text.
    // Supported encodings include ANSI, UTF-8, UTF-16 (LE/BE), and UTF-32 (LE/BE).
    class TextDetector
    {
    public:
        static bool isText(span<const std::byte> buffer)
        {
            EncodingDetector encodingDetector(buffer);

            buffer = buffer.subspan(encodingDetector.getBomLength());

            switch (encodingDetector.getEncoding())
            {
            case EncodingDetector::ANSI:
            case EncodingDetector::UTF8:
                return isValidTextLE(span_cast<const uint8_t>(buffer));

            case EncodingDetector::UTF16LE:
                return isValidTextLE(span_cast<const uint16_t>(buffer));

            case EncodingDetector::UTF16BE:
                return isValidTextBE(span_cast<const uint16_t>(buffer));

            case EncodingDetector::UTF32LE:
                return isValidTextLE(span_cast<const uint32_t>(buffer));

            case EncodingDetector::UTF32BE:
                return isValidTextBE(span_cast<const uint32_t>(buffer));

            default:
                return false;
            }
        }

    private:
        static uint16_t swapBytes(uint16_t val)
        {
            return  _byteswap_ushort(val);
        }

        static uint32_t swapBytes(uint32_t val)
        {
            return  _byteswap_ulong(val);
        }

        static bool isInvalidChar(auto ch)
        {
            return ch >= 0 && ch <= 0x1f && (ch != 0xa || ch != 0xd || ch != 0x9);
        }

        template<class T>
        static bool isValidTextLE(span<const T> buffer)
        {
            return ranges::none_of(buffer, [](auto ch) { return isInvalidChar(ch); });
        }

        template<class T>
        static bool isValidTextBE(span<const T> buffer)
        {
            return ranges::none_of(buffer, [](auto ch) { return isInvalidChar(swapBytes(ch)); });
        }
    };
}
