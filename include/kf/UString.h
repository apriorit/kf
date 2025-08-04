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
#pragma warning(suppress: 28160) // Must succeed pool allocations are forbidden. Allocation failures cause a system crash.
#pragma warning(suppress : 4996) // ExAllocatePoolWithTag is deprecated, use ExAllocatePool2
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

    private:
        UString(const UString&);
        UString& operator=(const UString&);

    private:
        enum { PoolTag = '++SU' };

    private:
        void* m_buffer;
    };
} // namespace
