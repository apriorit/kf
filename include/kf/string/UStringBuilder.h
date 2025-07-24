#pragma once
#include "UString.h"
#include <utility>

namespace kf
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // UStringBuilder - inspired by http://docs.oracle.com/javase/7/docs/api/java/lang/StringBuilder.html

    template<POOL_TYPE poolType>
    class UStringBuilder
    {
    public:  
        UStringBuilder()
        {
        }

        NTSTATUS reserve(int charLength)
        {
            return m_str.realloc(charLength * sizeof(wchar_t));
        }

        template<typename... Args>
        NTSTATUS append(_In_ const Args&... args)
        {
            NTSTATUS status = m_str.realloc(m_str.byteLength() + getRequiredSize(args...));
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            concat(args...);
            return STATUS_SUCCESS;
        }

        const USimpleString& string() const
        {
            return m_str;
        }

        UString<poolType>& string()
        {
            return m_str;
        }

    private:
        template<typename T, typename... Args>
        void concat(_In_ const T& arg, _In_ const Args&... args)
        {
            concat(arg);
            concat(args...);
        }

        template<typename T>
        void concat(_In_ const T& arg)
        {
            NTSTATUS status = m_str.concat(arg);
            ASSERT(NT_SUCCESS(status)); // Should always succeed as the buffer is preallocated.
            UNREFERENCED_PARAMETER(status);
        }

    private:
        template<typename T, typename... Args>
        static int getRequiredSize(_In_ const T& arg, _In_ Args... args)
        {
            return getRequiredSize(arg) + getRequiredSize(args...);
        }

        template<typename T>
        static int getRequiredSize(_In_ const T& arg)
        {
            return USimpleString(arg).byteLength();
        }

    private:
        UStringBuilder(const UStringBuilder&);
        UStringBuilder& operator=(const UStringBuilder&);

    private:
        UString<poolType> m_str;
    };
} // namespace
