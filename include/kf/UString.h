#pragma once
#include "USimpleString.h"
#include <utility>

namespace kf
{
    ///////////////////////////////////////////////////////////////////////////////////////////////////
    // UString - owning string for NT kernel

    template<POOL_TYPE poolType>
    class UString : public USimpleString
    {
    public:
        UString(const UString&) = delete;
        UString& operator=(const UString&) = delete;

        UString() : m_buffer(nullptr)
        {
        }

        UString(_Inout_ UString&& another) : USimpleString(std::move(another))
        {
            m_buffer = another.m_buffer;
            another.m_buffer = nullptr;
        }

        ~UString()
        {
            free();
        }

        NTSTATUS init(_In_ PCWSTR source)
        {
            UNICODE_STRING sourceString;
            ::RtlInitUnicodeString(&sourceString, source);

            return init(sourceString);
        }

        NTSTATUS init(_In_ PCWSTR source, int byteLength)
        {
            return init(UNICODE_STRING{ static_cast<USHORT>(byteLength), static_cast<USHORT>(byteLength), const_cast<PWSTR>(source) });
        }

        NTSTATUS init(_In_ const UNICODE_STRING& source)
        {
            NTSTATUS status = realloc(source.Length);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            RtlCopyMemory(m_buffer, source.Buffer, source.Length);
            setByteLength(source.Length);

            return STATUS_SUCCESS;
        }

        NTSTATUS init(_In_ const ANSI_STRING& source)
        {
            NTSTATUS status = realloc(RtlAnsiStringToUnicodeSize(reinterpret_cast<PCANSI_STRING>(const_cast<PANSI_STRING>(&source))));
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            return RtlAnsiStringToUnicodeString(&string(), reinterpret_cast<PCANSI_STRING>(const_cast<PANSI_STRING>(&source)), false);
        }

        NTSTATUS init(_In_ const USimpleString& source)
        {
            return init(source.string());
        }

        NTSTATUS realloc(_In_ int newByteLength)
        {
            void* newBuffer = nullptr;

            if (newByteLength > 0)
            {
// 28160: Must succeed pool allocations are forbidden. Allocation failures cause a system crash.
// 4996: ExAllocatePoolWithTag is deprecated, use ExAllocatePool2
#pragma warning(suppress: 28160 4996) 
                newBuffer = ::ExAllocatePoolWithTag(poolType, newByteLength, PoolTag);

                if (!newBuffer)
                {
                    return STATUS_INSUFFICIENT_RESOURCES;
                }
            }

            if (!isEmpty())
            {
                const int bytesToCopy = min(byteLength(), newByteLength);

#pragma warning(suppress: 6387) // Invalid parameter value 'newBuffer' could be '0'
                RtlCopyMemory(newBuffer, buffer(), bytesToCopy);

                free();
                setString(newBuffer, bytesToCopy, newByteLength);
            }
            else
            {
                free();
                setString(newBuffer, 0, newByteLength);
            }

            m_buffer = newBuffer;

            return STATUS_SUCCESS;
        }

        void free()
        {
            if (m_buffer)
            {
                ::ExFreePoolWithTag(m_buffer, PoolTag);
                m_buffer = nullptr;

                empty();
            }
        }

        UString& operator=(_Inout_ UString&& another)
        {
            if (this != &another)
            {
                free();

                USimpleString::operator=(std::move(another));

                m_buffer = another.m_buffer;
                another.m_buffer = nullptr;
            }

            return *this;
        }
        
        NTSTATUS concat(_In_ const USimpleString& str)
        {
            return ::RtlAppendUnicodeStringToString(&m_str, &str.string());
        }

        // concat may return STATUS_BUFFER_TOO_SMALL if internal buffer is not large enough.
        // Use realloc to increase the size of the buffer before calling concat.
        NTSTATUS concat(_In_ PCWSTR str)
        {
            return ::RtlAppendUnicodeToString(&m_str, str);
        }

        // format can return STATUS_BUFFER_OVERFLOW if the formatted string exceeds the maximum length.
        // Use realloc to increase the size of the buffer before calling format.
        NTSTATUS format(_In_ LPCWSTR fmt, ...)
        {
            va_list va;
            va_start(va, fmt);
            NTSTATUS status = format(fmt, va);
            va_end(va);

            return status;
        }

        // format can return STATUS_BUFFER_OVERFLOW if the formatted string exceeds the maximum length.
        // Use realloc to increase the size of the buffer before calling format.
        NTSTATUS format(_In_ LPCWSTR fmt, _In_ va_list va)
        {
            // TODO: change _vsnwprintf to something more secure
            const int charsWritten = _vsnwprintf(m_str.Buffer, maxCharLength(), fmt, va);
            if (charsWritten < 0)
            {
                return STATUS_BUFFER_OVERFLOW;
            }

            setCharLength(charsWritten);
            return STATUS_SUCCESS;
        }


        NTSTATUS toUpperCase()
        {
            return isEmpty() ? STATUS_SUCCESS : ::RtlUpcaseUnicodeString(const_cast<PUNICODE_STRING>(&string()), &string(), FALSE);
        }

        NTSTATUS toLowerCase()
        {
            return isEmpty() ? STATUS_SUCCESS : ::RtlDowncaseUnicodeString(const_cast<PUNICODE_STRING>(&string()), &string(), FALSE);
        }

    private:
        enum { PoolTag = '++SU' };

    private:
        void* m_buffer;
    };
} // namespace
