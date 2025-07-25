#pragma once
#include <utility>
#include <span>
#include <ntstrsafe.h>

namespace kf
{
    using namespace std;

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // ASimpleString - non-owning string for NT kernel, inspired by http://docs.oracle.com/javase/7/docs/api/java/lang/String.html

    class ASimpleString

    {
    public:
        ASimpleString();

        template<class T, size_t Extent>
        ASimpleString(span<T, Extent> buffer);

        ASimpleString(_In_ const ANSI_STRING& str)
        {
            setString(str.Buffer, str.Length, str.MaximumLength);
        }

        ASimpleString(_In_ const char* string)
        {
            setString(const_cast<char*>(string), static_cast<int>(strlen(string)));
        }

        const ANSI_STRING& string() const;
        ANSI_STRING& string();

        void empty();

        void setString(_In_reads_bytes_(byteLength) void* buffer, _In_ int byteLength);
        void setString(_In_reads_bytes_(maxByteLength) void* buffer, _In_ int byteLength, _In_ int maxByteLength);

        //Returns true ifand only if this string contains the specified sequence of char values.
        bool contains(const ASimpleString& str) const;

        //Returns the index within this string of the first occurrence of the specified substring.
        int	indexOf(const ASimpleString& str) const;

        //Returns the index within this string of the first occurrence of the specified substring, starting at the specified index.
        int indexOf(const ASimpleString& str, int fromIndex) const;

        int charLength() const;

        int byteLength() const
        {
            return charLength();
        }

        const char& charAt(int index) const
        {
            return m_str.Buffer[index];
        }

        ASimpleString split(char separator, _Inout_ int& fromIndex) const
        {
            int originalFromIndex = fromIndex;

            fromIndex = indexOf(separator, fromIndex);
            if (fromIndex < 0)
            {
                return substring(originalFromIndex);
            }

            return substring(originalFromIndex, fromIndex++);
        }

        ASimpleString trimRight(char ch) const
        {
            int endIndex = charLength();
            while (endIndex > 0 && charAt(endIndex - 1) == ch)
            {
                --endIndex;
            }

            return substring(0, endIndex);
        }

        ASimpleString trimLeft(char ch) const
        {
            int startIndex = 0;
            while (startIndex < charLength() && charAt(startIndex) == ch)
            {
                ++startIndex;
            }

            return substring(startIndex);
        }

        ASimpleString trimLeft(const ASimpleString& chars) const
        {
            int startIndex = 0;
            while (startIndex < charLength())
            {
                bool found = false;

                for (auto ch : chars)
                {
                    if (charAt(startIndex) == ch)
                    {
                        ++startIndex;
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    break;
                }
            }

            return substring(startIndex);
        }

        ASimpleString substring(_In_ int beginIndex) const
        {
            return substring(beginIndex, charLength());
        }

        ASimpleString substring(_In_ int beginIndex, _In_ int endIndex) const
        {
            ASSERT(charLength() >= endIndex);
            ASSERT(beginIndex <= endIndex);

            ASimpleString str;

            if (beginIndex < endIndex)
            {
                str.setString(const_cast<char*>(&charAt(beginIndex)), (endIndex - beginIndex) * sizeof(char), (charLength() - beginIndex) * sizeof(char));
            }

            return str;
        }

        int indexOf(char ch, int fromIndex) const
        {
            for (int i = fromIndex; i < charLength(); ++i)
            {
                if (charAt(i) == ch)
                {
                    return i;
                }
            }

            return -1;
        }

        bool isEmpty() const
        {
            return !m_str.Length;
        }

        bool equals(_In_ const ANSI_STRING& str) const
        {
            return !!RtlEqualString(&str, &m_str, false);
        }

        const char* begin() const
        {
            return m_str.Buffer;
        }

        const char* end() const
        {
            return m_str.Buffer + charLength();
        }

    private:
        ANSI_STRING m_str;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // ASimpleString - inline

    inline ASimpleString::ASimpleString()
    {
        empty();
    }

    template<class T, size_t Extent>
    inline ASimpleString::ASimpleString(span<T, Extent> buffer)
    {
        setString(const_cast<void*>(reinterpret_cast<const void*>(buffer.data())), static_cast<int>(buffer.size_bytes()));
    }

    inline const ANSI_STRING& ASimpleString::string() const
    {
        return m_str;
    }

    inline ANSI_STRING& ASimpleString::string()
    {
        return m_str;
    }

    inline void ASimpleString::setString(_In_reads_bytes_(byteLength) void* buffer, _In_ int byteLength)
    {
        setString(buffer, byteLength, byteLength);
    }

    inline void ASimpleString::setString(_In_reads_bytes_(maxByteLength) void* buffer, _In_ int byteLength, _In_ int maxByteLength)
    {
        ASSERT(maxByteLength >= byteLength);

        m_str.Buffer = static_cast<PCH>(buffer);
        m_str.Length = static_cast<USHORT>(byteLength);
        m_str.MaximumLength = static_cast<USHORT>(maxByteLength);
    }

    inline void ASimpleString::empty()
    {
        setString(nullptr, 0, 0);
    }

    inline bool ASimpleString::contains(const ASimpleString& str) const
    {
        return indexOf(str) != -1;
    }

    inline int ASimpleString::indexOf(const ASimpleString& str) const
    {
        return indexOf(str, 0);
    }

    inline int ASimpleString::indexOf(const ASimpleString& str, int fromIndex) const
    {
        if (str.m_str.Length == 0)
        {
            return 0;
        }

        int equalLen = 0;
        auto current = str.m_str.Buffer;

        for (auto i = fromIndex; i < m_str.Length; ++i)
        {
            if (m_str.Buffer[i] == *current)
            {
                ++equalLen;
                ++current;
            }
            else
            {
                i -= equalLen;
                equalLen = 0;
                current = str.m_str.Buffer;
            }

            if (equalLen == str.m_str.Length)
            {
                return i + 1 - equalLen;
            }
        }

        return -1;
    }

    inline int ASimpleString::charLength() const
    {
        return m_str.Length;
    }
}
