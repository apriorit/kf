#pragma once
#include <span>
#include <cstddef>

namespace kf
{

    //////////////////////////////////////////////////////////////////////////////////
    // EncodingDetector class identifies the encoding of a buffer.
    // It detects ANSI, UTF-8, UTF-16 (LE/BE), and UTF-32 (LE/BE) encodings,
    // based on the presence of a Byte Order Mark(BOM) or by analyzing byte patterns.
    class EncodingDetector
    {
    public:
        EncodingDetector(std::span<const std::byte> buffer);

        enum Encoding
        {
            Unknown,
            ANSI,
            UTF8,
            UTF16LE,
            UTF16BE,
            UTF32LE,
            UTF32BE,
        };

        Encoding getEncoding() const;
        int getBomLength() const;
        
        enum { kMaximumBomLength = 4 };
        enum { kMinimalBufferSize = kMaximumBomLength };

    private:
        bool detectBom(std::span<const std::byte, kMaximumBomLength> bomBytes);
        bool detectUtf16(std::span<const std::byte> buffer);

    private:
        Encoding m_encoding = Unknown;
        int m_bomLength = 0;
    };

    inline EncodingDetector::EncodingDetector(std::span<const std::byte> buffer) : m_encoding(), m_bomLength()
    {
        if (buffer.size() < kMinimalBufferSize)
        {
            return;
        }

        if (detectBom(buffer.first<kMaximumBomLength>()))
        {
            return;
        }

        if (detectUtf16(buffer))
        {
            return;
        }

        //TODO: Add check for ANSI encoding
        m_encoding = ANSI;
    }

    inline bool EncodingDetector::detectBom(std::span<const std::byte, kMaximumBomLength> bomBytes)
    {
        if (bomBytes[0] == std::byte(0xff) && bomBytes[1] == std::byte(0xfe) && (bomBytes[2] != std::byte(0) || bomBytes[3] != std::byte(0)))
        {
            m_bomLength = 2;
            m_encoding = UTF16LE;
            return true;
        }

        if (bomBytes[0] == std::byte(0xfe) && bomBytes[1] == std::byte(0xff))
        {
            m_bomLength = 2;
            m_encoding = UTF16BE;
            return true;
        }

        if (bomBytes[0] == std::byte(0xef) && bomBytes[1] == std::byte(0xbb) && bomBytes[2] == std::byte(0xbf))
        {
            m_bomLength = 3;
            m_encoding = UTF8;
            return true;
        }

        if (bomBytes[0] == std::byte(0xff) && bomBytes[1] == std::byte(0xfe) && bomBytes[2] == std::byte(0) && bomBytes[3] == std::byte(0))
        {
            m_bomLength = 4;
            m_encoding = UTF32LE;
            return true;
        }

        if (bomBytes[0] == std::byte(0) && bomBytes[1] == std::byte(0) && bomBytes[2] == std::byte(0xfe) && bomBytes[3] == std::byte(0xff))
        {
            m_bomLength = 4;
            m_encoding = UTF32BE;
            return true;
        }

        return false;
    }

    inline bool EncodingDetector::detectUtf16(std::span<const std::byte> buffer)
    {
        int zeros[2] = {};

        for (size_t i = 0; i < buffer.size(); ++i)
        {
            if (buffer[i] == std::byte(0))
            {
                ++zeros[i % 2];
            }
        }

        if (zeros[1] > zeros[0] * 4)
        {
            m_encoding = UTF16LE;
            return true;
        }

        if (zeros[0] > zeros[1] * 4)
        {
            m_encoding = UTF16BE;
            return true;
        }

        return false;
    }

    inline auto EncodingDetector::getEncoding() const -> Encoding
    {
        return m_encoding;
    }

    inline int EncodingDetector::getBomLength() const
    {
        return m_bomLength;
    }
}
