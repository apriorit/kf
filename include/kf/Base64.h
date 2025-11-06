#pragma once
#include "USimpleString.h"
#include <span>

namespace kf
{

    // original implementation is taken from https://github.com/adamvr/arduino-base64/blob/master/Base64.cpp

    //////////////////////////////////////////////////////////////////////////////////
    // Base64 is utility for decoding Base64-encoded strings.
    // Only ASCII-compatible Base64 is supported.
    // Decoded buffer contains bytes as 'char', not 'WCHAR'.
    // If the string contains many '=' characters, the decoded length may be negative.
    class Base64
    {
    public:
        static int decodeLen(const USimpleString& input) 
        {
            if (input.isEmpty())
            {
                return 0;
            }

            int numEq = 0;
            
            for (int i = input.charLength() - 1; i >= 0 && static_cast<char>(input.charAt(i)) == '='; --i)
            {
                numEq++;
            }

            return ((6 * input.charLength()) / 8) - numEq;
        }

        static int decode(const USimpleString& input, span<std::byte> output)
        {
            int inputIdx = 0;
            int a4Idx = 0;
            int outputIdx = 0;
            uint8_t a3[3];
            uint8_t a4[4];

            int decodedLen = decodeLen(input);
            if (decodedLen < 0 || static_cast<int>(output.size()) < decodedLen)
            {
                return -1;
            }

            while (inputIdx < input.charLength()) 
            {
                auto ch = static_cast<char>(input.charAt(inputIdx++));

                if (ch == '=') 
                {
                    break;
                }

                a4[a4Idx++] = ch;
                if (a4Idx == 4)
                {
                    for (int i = 0; i < 4; i++) 
                    {
                        a4[i] = b64_lookup(a4[i]);
                    }

                    a4_to_a3(a3, a4);

                    for (int i = 0; i < 3; i++) 
                    {
                        output[outputIdx++] = std::byte(a3[i]);
                    }
                    a4Idx = 0;
                }
            }

            if (a4Idx)
            {
                for (int j = a4Idx; j < 4; j++)
                {
                    a4[j] = 0;
                }

                for (int j = 0; j < 4; j++)
                {
                    a4[j] = b64_lookup(a4[j]);
                }

                a4_to_a3(a3, a4);

                for (int j = 0; j < a4Idx - 1; j++)
                {
                    output[outputIdx++] = std::byte(a3[j]);
                }
            }

            return outputIdx;
        }

    private:
        static void a3_to_a4(uint8_t* a4, uint8_t* a3)
        {
            a4[0] = (a3[0] & 0xfc) >> 2;
            a4[1] = ((a3[0] & 0x03) << 4) + ((a3[1] & 0xf0) >> 4);
            a4[2] = ((a3[1] & 0x0f) << 2) + ((a3[2] & 0xc0) >> 6);
            a4[3] = (a3[2] & 0x3f);
        }

        static void a4_to_a3(uint8_t* a3, uint8_t* a4)
        {
            a3[0] = (a4[0] << 2) + ((a4[1] & 0x30) >> 4);
            a3[1] = ((a4[1] & 0xf) << 4) + ((a4[2] & 0x3c) >> 2);
            a3[2] = ((a4[2] & 0x3) << 6) + a4[3];
        }

        static uint8_t b64_lookup(char c)
        {
            if (c >= 'A' && c <= 'Z')
            {
                return c - 'A';
            }
            if (c >= 'a' && c <= 'z')
            {
                return c - 71;
            }
            if (c >= '0' && c <= '9')
            {
                return c + 4;
            }
            if (c == '+')
            {
                return 62;
            }
            if (c == '/')
            {
                return 63;
            }

            return static_cast<uint8_t>(-1);
        }

    private:
        static const char m_b64Alphabet[];
    };

    __declspec(selectany) const char Base64::m_b64Alphabet[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";
}
