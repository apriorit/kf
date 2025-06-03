#pragma once
#include "BitmapIterator.h"

namespace kf
{
    template<POOL_TYPE poolType>
    class Bitmap
    {
    public:
        Bitmap()
        {
        }

        ~Bitmap()
        {
            free();
        }

        Bitmap(const Bitmap&) = delete;
        Bitmap& operator=(const Bitmap&) = delete;

        // IRQL <= APC_LEVEL for PagedPool and on Windows 7 and earlier
        NTSTATUS init(ULONG size)
        {
            if (this->size())
            {
                free();
            }

            // Buffer must be an integer multiple of sizeof(ULONG) bytes
            const ULONG bufferSize = size % sizeof(ULONG) ? (size / sizeof(ULONG) + 1) * sizeof(ULONG) : size;
            auto buffer = ::ExAllocatePoolWithTag(poolType, bufferSize, PoolTag);
            if (!buffer)
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }
            RtlZeroMemory(buffer, bufferSize);

            m_header = reinterpret_cast<PRTL_BITMAP>(::ExAllocatePoolWithTag(poolType, sizeof(RTL_BITMAP), PoolTag));
            if (!m_header)
            {
                ::ExFreePoolWithTag(buffer, PoolTag);
                return STATUS_INSUFFICIENT_RESOURCES;
            }
            RtlZeroMemory(m_header, sizeof(RTL_BITMAP));

            m_buffer = buffer;
            m_size = size;

            RtlInitializeBitMap(m_header, static_cast<PULONG>(m_buffer), m_size);
            return STATUS_SUCCESS;
        }

        ULONG size() const
        {
            return m_size;
        }

        void setBits(ULONG startingIndex, ULONG numberToSet)
        {
            RtlSetBits(m_header, startingIndex, numberToSet);
        }

        void clearBits(ULONG startingIndex, ULONG numberToSet)
        {
            RtlClearBits(m_header, startingIndex, numberToSet);
        }

        void setAll()
        {
            RtlSetAllBits(m_header);
        }

        void clearAll()
        {
            RtlClearAllBits(m_header);
        }

        bool areBitsSet(ULONG startingIndex, ULONG size)
        {
            return RtlAreBitsSet(m_header, startingIndex, size);
        }

        bool areBitsClear(ULONG startingIndex, ULONG size)
        {
            return RtlAreBitsClear(m_header, startingIndex, size);
        }

        ULONG numberOfSetBits()
        {
            return RtlNumberOfSetBits(m_header);
        }

        ULONG numberOfClearBits()
        {
            return RtlNumberOfClearBits(m_header);
        }

        BitmapRangeIterator rangeIterator(ULONG startingIndex = 0)
        {
            return BitmapRangeIterator(m_header, startingIndex);
        }

    private:
        void free()
        {
            if (m_buffer)
            {
                ::ExFreePoolWithTag(m_buffer, PoolTag);
                m_buffer = nullptr;
                m_size = 0;
            }

            if (m_header)
            {
                ExFreePoolWithTag(m_header, PoolTag);
                m_header = nullptr;
            }
        }

    private:
        enum { PoolTag = '++MB' };

    private:
        void* m_buffer = nullptr;
        ULONG m_size = 0;
        PRTL_BITMAP m_header = nullptr;
    };
}
