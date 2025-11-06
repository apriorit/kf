#pragma once
#include "USimpleString.h"
#include "ASimpleString.h"
#include <span>
#include <array>

namespace kf
{

    ////////////////////////////////////////////////////////////////////////////
    // The Hex class provides functionality to encode binary data into hexadecimal string representation
    // and decode hexadecimal strings back into binary data.
    class Hex
    {
    public:
        static int encodeLen(span<const std::byte> input)
        {
            return static_cast<int>(input.size() * 2);
        }

        static bool encode(span<const std::byte> input, _Out_ USimpleString& output)
        {
            if (output.maxByteLength() < encodeLen(input))
            {
                return false;
            }

            output.setCharLength(0);

            for (auto b : input)
            {
                const auto& hexByte = toHex(static_cast<uint8_t>(b));
                output.charAt(output.charLength()) = hexByte[0];
                output.charAt(output.charLength() + 1) = hexByte[1];

                output.setCharLength(output.charLength() + 2);
            }

            return true;
        }

        static int decodeLen(std::span<const char> input)
        {
            return static_cast<int>(input.size() / 2);
        }

        static int decodeLen(const ASimpleString& input)
        {
            return input.charLength() / 2;
        }

        static bool decode(const ASimpleString& input, _Out_ span<std::byte>& output)
        {
            if (output.size() != static_cast<size_t>(decodeLen(input)))
            {
                return false;
            }

            for (int i = 0; i < input.charLength() / 2; ++i)
            {
                int res = fromHex({ input.charAt(i * 2), input.charAt(i * 2 + 1) });
                if (res < 0)
                {
                    return false;
                }

                output[i] = static_cast<std::byte>(res);
            }

            return true;
        }

    private:
        static inline const char kHexArray[] = "0123456789ABCDEF";

        static std::array<wchar_t, 2> toHex(uint8_t b)
        {
            return { static_cast<wchar_t>(kHexArray[b >> 4]), static_cast<wchar_t>(kHexArray[b & 0xF]) };
        }

        static int fromHex(const std::array<const char, 2>& hexByte)
        {
            std::array<int, 2> digits;

            for (int i = 0; i < 2; ++i)
            {
                digits[i] = fromHex(hexByte[i]);
                if (digits[i] < 0)
                {
                    return -1;
                }
            }

            return digits[0] << 4 | digits[1];
        }

        static int fromHex(char ch)
        {
            if (ch >= '0' && ch <= '9')
            {
                return ch - '0';
            }
            else if (ch >= 'a' && ch <= 'f')
            {
                return ch - 'a' + 10;
            }
            else if (ch >= 'A' && ch <= 'F')
            {
                return ch - 'A' + 10;
            }

            return -1;
        }
    };
}
