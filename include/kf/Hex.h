#pragma once
#include <span>
#include <array>
#include "ASimpleString.h"
#include "USimpleString.h"

namespace kf
{
    using namespace std;

    class Hex
    {
    public:
        static constexpr int kEncodedSizeMultiplier = 2;

        static int encodeLen(span<const std::byte> input)
        {
            return static_cast<int>(input.size() * kEncodedSizeMultiplier);
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

        static int decodeLen(span<const char> input)
        {
            return static_cast<int>(input.size() / kEncodedSizeMultiplier);
        }

        static int decodeLen(const ASimpleString& input)
        {
            return input.charLength() / kEncodedSizeMultiplier;
        }

        static int decodeLen(const USimpleString& input)
        {
            return input.charLength() / kEncodedSizeMultiplier;
        }

        // TODO: join two decode method to the single one after making base class for both simple strings
        static bool decode(const ASimpleString& input, _Out_ span<std::byte>& output)
        {
            if (output.size() != static_cast<size_t>(decodeLen(input)))
            {
                return false;
            }

            for (int i = 0; i < input.charLength() / kEncodedSizeMultiplier; ++i)
            {
                int res = fromHex({ input.charAt(i * kEncodedSizeMultiplier), input.charAt(i * kEncodedSizeMultiplier + 1) });
                if (res < 0)
                {
                    return false;
                }

                output[i] = static_cast<std::byte>(res);
            }

            return true;
        }

        static bool decode(const USimpleString& input, _Out_ span<std::byte>& output)
        {
            if (input.charLength() % kEncodedSizeMultiplier != 0 ||
                output.size() < decodeLen(input))
            {
                return false;
            }

            char chars[2] = {};
            for (int i = 0; i < input.charLength(); i += kEncodedSizeMultiplier)
            {
                chars[0] = static_cast<char>(input.charAt(i));
                chars[1] = static_cast<char>(input.charAt(i + 1));
                int res = fromHex({ chars[0], chars[1] });
                if (res < 0)
                {
                    return false;
                }

                output[i/kEncodedSizeMultiplier] = static_cast<std::byte>(res);
            }

            return true;
        }

    private:
        static inline const char kHexArray[] = "0123456789ABCDEF";

        static array<wchar_t, 2> toHex(uint8_t b)
        {
            return { static_cast<wchar_t>(kHexArray[b >> 4]), static_cast<wchar_t>(kHexArray[b & 0xF]) };
        }

        static int fromHex(const array<const char, 2>& hexByte)
        {
            array<int, 2> digits;

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
