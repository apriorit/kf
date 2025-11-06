#pragma once
#include <utility>
#include <span>
#include <ntstrsafe.h>

namespace kf
{

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // USimpleString - non-owning string for NT kernel, inspired by http://docs.oracle.com/javase/7/docs/api/java/lang/String.html

    class USimpleString

    {
    public:
        USimpleString();
        USimpleString(_In_ const WCHAR* str);
        USimpleString(_In_ const WCHAR* str, _In_ int maxCharLength);
        USimpleString(_In_ const UNICODE_STRING& str);
        USimpleString(_In_reads_bytes_(byteLength) const void* buffer, _In_ int byteLength);

        template<class T, std::size_t Extent>
        USimpleString(std::span<T, Extent> buffer);

        USimpleString(_Inout_ USimpleString&& another);
        USimpleString(_In_ const USimpleString& another);

        static const USimpleString& fromUnicodeString(_In_ const UNICODE_STRING& str);

        const UNICODE_STRING& string() const;
        UNICODE_STRING& string();
        void setString(_In_reads_bytes_(byteLength) void* buffer, _In_ int byteLength);
        void setString(_In_reads_bytes_(maxByteLength) void* buffer, _In_ int byteLength, _In_ int maxByteLength);
        void setString(_In_ const UNICODE_STRING& str);
        void setString(_In_ const USimpleString& str);

        bool isEmpty() const;
        void empty();

        int byteLength() const;
        int maxByteLength() const;
        int charLength() const;
        int maxCharLength() const;

        void setByteLength(_In_ int newByteLength);
        void setCharLength(_In_ int newCharLength);

        PCWCH buffer() const;
        PWCH buffer();

        const WCHAR& charAt(_In_ int index) const;
        WCHAR& charAt(_In_ int index);

        WCHAR* begin();
        WCHAR* end();

        const WCHAR* begin() const;
        const WCHAR* end() const;

        // Comparison
        int compareTo(_In_ const UNICODE_STRING& str) const;
        int compareTo(_In_ const USimpleString& str) const;
        int compareToIgnoreCase(_In_ const UNICODE_STRING& str) const;
        int compareToIgnoreCase(_In_ const USimpleString& str) const;

        bool equals(_In_ const UNICODE_STRING& str) const;
        bool equals(_In_ const USimpleString& str) const;
        bool equalsIgnoreCase(_In_ const UNICODE_STRING& str) const;
        bool equalsIgnoreCase(_In_ const USimpleString& str) const;

        bool operator<(_In_ const USimpleString& another) const;
        bool operator==(_In_ const USimpleString& another) const;

        USimpleString& operator=(_Inout_ USimpleString&& another);

        USimpleString split(_In_ WCHAR separator, _Inout_ int& fromIndex) const;
        bool contains(const USimpleString& str) const;

        int indexOf(_In_ WCHAR ch, _In_ int fromIndex = 0) const;

        //Returns the index within this string of the first occurrence of the specified substring, starting at the specified index.
        int indexOf(_In_ const USimpleString& str, _In_ int fromIndex = 0) const;

        int lastIndexOf(_In_ WCHAR ch) const;
        int lastIndexOf(_In_ WCHAR ch, _In_ int fromIndex) const;

        USimpleString substring(_In_ int beginIndex) const;
        USimpleString substring(_In_ int beginIndex, _In_ int endIndex) const;

        USimpleString trim(_In_ WCHAR ch) const;

        USimpleString trimRight(_In_ WCHAR ch) const;
        USimpleString trimLeft(_In_ WCHAR ch) const;

        USimpleString trimLeft(_In_ const USimpleString& chars) const;

        bool startsWith(_In_ const USimpleString& str) const;
        bool startsWithIgnoreCase(_In_ const USimpleString& str) const;

        bool endsWith(_In_ const USimpleString& str) const;
        bool endsWithIgnoreCase(_In_ const USimpleString& str) const;

        NTSTATUS toUpperCase();
        NTSTATUS toLowerCase();

        bool matches(_In_ const USimpleString& expression) const;
        bool matchesIgnoreCase(_In_ const USimpleString& expression) const;
        LONG toLong(_In_ ULONG base) const;

        NTSTATUS concat(_In_ const USimpleString& str)
        {
            return ::RtlAppendUnicodeStringToString(&m_str, &str.string());
        }

        NTSTATUS concat(_In_ PCWSTR str)
        {
            return ::RtlAppendUnicodeToString(&m_str, str);
        }

        template<std::size_t N>
        int copyTo(_Out_ WCHAR(&destination)[N]) const
        {
            return copyTo(N, destination);
        }

        int copyTo(_In_ int maxCharLength, _Out_cap_(maxCharLength) WCHAR* destination) const
        {
            const int charsToCopy = std::min(maxCharLength - 1, charLength());

            if (charsToCopy >= 0)
            {
                ::memcpy(destination, m_str.Buffer, charsToCopy * sizeof(WCHAR));
                destination[charsToCopy] = 0; // write termination null
            }

            return charsToCopy;
        }

        NTSTATUS format(_In_ LPCWSTR fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            NTSTATUS status = format(fmt, va);
            va_end(va);

            return status;
        }

        NTSTATUS format(_In_ LPCWSTR fmt, _In_ va_list va)
        {
            const int charsWritten = _vsnwprintf(m_str.Buffer, maxCharLength(), fmt, va);
            if (charsWritten < 0)
            {
                return STATUS_BUFFER_OVERFLOW;
            }

            setCharLength(charsWritten);
            return STATUS_SUCCESS;
        }

    public:
        struct LessIgnoreCase
        {
            bool operator()(_In_ const USimpleString& strLeft, _In_ const USimpleString& strRight) const;
        };

    private:
        UNICODE_STRING m_str;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // USimpleString - inline

    inline USimpleString::USimpleString()
    {
        empty();
    }

    inline USimpleString::USimpleString(_In_ const WCHAR* str)
    {
        ::RtlInitUnicodeString(&m_str, str);
    }

    inline USimpleString::USimpleString(_In_ const WCHAR* str, _In_ int maxCharLength)
    {
        std::size_t byteLength = 0;
        ::RtlStringCbLengthW(str, maxCharLength * sizeof(WCHAR), &byteLength);
        setString(const_cast<WCHAR*>(str), static_cast<int>(byteLength), maxCharLength * sizeof(WCHAR));
    }

    inline USimpleString::USimpleString(_In_ const UNICODE_STRING& str)
    {
        setString(str);
    }

    inline USimpleString::USimpleString(_In_reads_bytes_(byteLength) const void* buffer, _In_ int byteLength)
    {
        setString(const_cast<void*>(buffer), byteLength);
    }

    template<class T, std::size_t Extent>
    inline USimpleString::USimpleString(std::span<T, Extent> buffer)
    {
        setString(const_cast<void*>(reinterpret_cast<const void*>(buffer.data())), static_cast<int>(buffer.size_bytes()));
    }

    inline USimpleString::USimpleString(_Inout_ USimpleString&& another) : m_str(another.m_str)
    {
        another.empty();
    }

    inline USimpleString::USimpleString(_In_ const USimpleString& another) : m_str(another.m_str)
    {
    }

    inline const USimpleString& USimpleString::fromUnicodeString(_In_ const UNICODE_STRING& str)
    {
        return reinterpret_cast<const USimpleString&>(str);
    }

    inline const UNICODE_STRING& USimpleString::string() const
    {
        return m_str;
    }

    inline UNICODE_STRING& USimpleString::string()
    {
        return m_str;
    }

    inline void USimpleString::setString(_In_reads_bytes_(byteLength) void* buffer, _In_ int byteLength)
    {
        setString(buffer, byteLength, byteLength);
    }

    inline void USimpleString::setString(_In_reads_bytes_(maxByteLength) void* buffer, _In_ int byteLength, _In_ int maxByteLength)
    {
        ASSERT(maxByteLength >= byteLength);

        m_str.Buffer = static_cast<PWCH>(buffer);
        m_str.Length = static_cast<USHORT>(byteLength);
        m_str.MaximumLength = static_cast<USHORT>(maxByteLength);
    }

    inline void USimpleString::setString(_In_ const UNICODE_STRING& str)
    {
        setString(str.Buffer, str.Length, str.MaximumLength);
    }

    inline void USimpleString::setString(_In_ const USimpleString& str)
    {
        setString(str.string());
    }

    inline bool USimpleString::isEmpty() const
    {
        return !m_str.Length;
    }

    inline void USimpleString::empty()
    {
        setString(nullptr, 0, 0);
    }

    inline int USimpleString::byteLength() const
    {
        return m_str.Length;
    }

    inline int USimpleString::maxByteLength() const
    {
        return m_str.MaximumLength;
    }

    inline int USimpleString::charLength() const
    {
        return byteLength() / sizeof(WCHAR);
    }

    inline int USimpleString::maxCharLength() const
    {
        return maxByteLength() / sizeof(WCHAR);
    }

    inline void USimpleString::setByteLength(_In_ int newByteLength)
    {
        ASSERT(m_str.MaximumLength >= newByteLength);
        m_str.Length = static_cast<USHORT>(newByteLength);
    }

    inline void USimpleString::setCharLength(_In_ int newCharLength)
    {
        setByteLength(newCharLength * sizeof(WCHAR));
    }

    inline PCWCH USimpleString::buffer() const
    {
        return const_cast<USimpleString*>(this)->buffer();
    }

    inline PWCHAR USimpleString::buffer()
    {
        return m_str.Buffer;
    }

    inline const WCHAR& USimpleString::charAt(_In_ int index) const
    {
        return const_cast<USimpleString*>(this)->charAt(index);
    }

    inline WCHAR& USimpleString::charAt(_In_ int index)
    {
        ASSERT(maxCharLength() > index);
        return m_str.Buffer[index];
    }

    inline WCHAR* USimpleString::begin()
    {
        return m_str.Buffer;
    }

    inline const WCHAR* USimpleString::begin() const
    {
        return const_cast<USimpleString*>(this)->begin();
    }

    inline WCHAR* USimpleString::end()
    {
        return m_str.Buffer + charLength();
    }

    inline const WCHAR* USimpleString::end() const
    {
        return const_cast<USimpleString*>(this)->end();
    }

    // Comparison return code meaning:
    // Return code	Description
    // 0: this equals str
    // <0: this is less than str
    // >0: this is greater than str
    inline int USimpleString::compareTo(_In_ const UNICODE_STRING& str) const
    {
        return ::RtlCompareUnicodeString(&m_str, &str, FALSE);
    }

    inline int USimpleString::compareTo(_In_ const USimpleString& str) const
    {
        return compareTo(str.string());
    }

    inline int USimpleString::compareToIgnoreCase(_In_ const UNICODE_STRING& str) const
    {
        return ::RtlCompareUnicodeString(&m_str, &str, TRUE);
    }

    inline int USimpleString::compareToIgnoreCase(_In_ const USimpleString& str) const
    {
        return compareToIgnoreCase(str.string());
    }

    inline bool USimpleString::equals(_In_ const UNICODE_STRING& str) const
    {
        return !!::RtlEqualUnicodeString(&m_str, &str, FALSE);
    }

    inline bool USimpleString::equals(_In_ const USimpleString& str) const
    {
        return equals(str.string());
    }

    inline bool USimpleString::equalsIgnoreCase(_In_ const UNICODE_STRING& str) const
    {
        return !!::RtlEqualUnicodeString(&m_str, &str, TRUE);
    }

    inline bool USimpleString::equalsIgnoreCase(_In_ const USimpleString& str) const
    {
        return equalsIgnoreCase(str.string());
    }

    // Case sensitive lexicographical comparison
    inline bool USimpleString::operator<(_In_ const USimpleString& another) const
    {
        return compareTo(another) < 0;
    }

    // Case sensitive lexicographical comparison
    inline bool USimpleString::operator==(_In_ const USimpleString& another) const
    {
        return equals(another);
    }

    inline USimpleString& USimpleString::operator=(_Inout_ USimpleString&& another)
    {
        if (this != &another)
        {
            setString(another);
            another.empty();
        }

        return *this;
    }

    inline USimpleString USimpleString::split(_In_ WCHAR separator, _Inout_ int& fromIndex) const
    {
        int originalFromIndex = fromIndex;

        fromIndex = indexOf(separator, fromIndex);
        if (fromIndex < 0)
        {
            return substring(originalFromIndex);
        }

        return substring(originalFromIndex, fromIndex++);
    }

    inline bool USimpleString::contains(const USimpleString& str) const
    {
        return indexOf(str) >= 0;
    }

    inline int USimpleString::indexOf(const USimpleString& str, int fromIndex) const
    {
        if (str.charLength() > charLength())
        {
            return -1;
        }

        int lastSearchIndex = charLength() - str.charLength();
        for (int i = fromIndex; i <= lastSearchIndex; ++i)
        {
            if (substring(i, i + str.charLength()) == str)
            {
                return i;
            }
        }

        return -1;
    }

    inline int USimpleString::indexOf(_In_ WCHAR ch, _In_ int fromIndex) const
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

    inline int USimpleString::lastIndexOf(_In_ WCHAR ch) const
    {
        return lastIndexOf(ch, charLength() - 1);
    }

    inline int USimpleString::lastIndexOf(_In_ WCHAR ch, _In_ int fromIndex) const
    {
        ASSERT(fromIndex <= charLength() - 1);

        for (int i = fromIndex; i >= 0; --i)
        {
            if (charAt(i) == ch)
            {
                return i;
            }
        }

        return -1;
    }

    inline USimpleString USimpleString::substring(_In_ int beginIndex) const
    {
        return substring(beginIndex, charLength());
    }

    inline USimpleString USimpleString::substring(_In_ int beginIndex, _In_ int endIndex) const
    {
        ASSERT(charLength() >= endIndex);
        ASSERT(beginIndex <= endIndex);

        USimpleString str;

        if (beginIndex < endIndex)
        {
            str.setString(const_cast<WCHAR*>(&charAt(beginIndex)), (endIndex - beginIndex) * sizeof(WCHAR), (charLength() - beginIndex) * sizeof(WCHAR));
        }

        return str;
    }

    inline USimpleString USimpleString::trim(_In_ WCHAR ch) const
    {
        return trimLeft(ch).trimRight(ch);
    }

    inline USimpleString USimpleString::trimRight(_In_ WCHAR ch) const
    {
        int endIndex = charLength();
        while(endIndex > 0 && charAt(endIndex - 1) == ch)
        {
            --endIndex;
        }

        return substring(0, endIndex);
    }

    inline USimpleString USimpleString::trimLeft(_In_ WCHAR ch) const
    {
        int startIndex = 0;
        while (startIndex < charLength() && charAt(startIndex) == ch)
        {
            ++startIndex;
        }

        return substring(startIndex);
    }

    inline USimpleString USimpleString::trimLeft(_In_ const USimpleString& chars) const
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

    inline bool USimpleString::startsWith(_In_ const USimpleString& str) const
    {
        return !!::RtlPrefixUnicodeString(&str.string(), &m_str, false);
    }

    inline bool USimpleString::startsWithIgnoreCase(_In_ const USimpleString& str) const
    {
        return !!::RtlPrefixUnicodeString(&str.string(), &m_str, true);
    }

    inline bool USimpleString::endsWith(_In_ const USimpleString& str) const
    {
        int idx = charLength() - str.charLength();
        return idx >= 0 && substring(idx).equals(str);
    }

    inline bool USimpleString::endsWithIgnoreCase(_In_ const USimpleString& str) const
    {
        int idx = charLength() - str.charLength();
        return idx >= 0 && substring(idx).equalsIgnoreCase(str);
    }

    inline NTSTATUS USimpleString::toUpperCase()
    {
        return isEmpty() ? STATUS_SUCCESS : RtlUpcaseUnicodeString(const_cast<PUNICODE_STRING>(&string()), &string(), FALSE);
    }

    inline NTSTATUS USimpleString::toLowerCase()
    {
        return isEmpty() ? STATUS_SUCCESS : RtlDowncaseUnicodeString(const_cast<PUNICODE_STRING>(&string()), &string(), FALSE);
    }

    inline bool USimpleString::matches(_In_ const USimpleString& expression) const
    {
        return !!::FsRtlIsNameInExpression(const_cast<PUNICODE_STRING>(&expression.string()), const_cast<PUNICODE_STRING>(&string()), false, NULL);
    }

    inline bool USimpleString::matchesIgnoreCase(_In_ const USimpleString& expression) const
    {
        return !!::FsRtlIsNameInExpression(const_cast<PUNICODE_STRING>(&expression.string()), const_cast<PUNICODE_STRING>(&string()), true, NULL);
    }

    inline bool USimpleString::LessIgnoreCase::operator()(_In_ const USimpleString& strLeft, _In_ const USimpleString& strRight) const
    {
        return strLeft.compareToIgnoreCase(strRight) < 0;
    }

    inline LONG USimpleString::toLong(_In_ ULONG base) const
    {
        ULONG value = 0;
        ::RtlUnicodeStringToInteger(&m_str, base, reinterpret_cast<PULONG>(&value));

        return value;
    }
}
