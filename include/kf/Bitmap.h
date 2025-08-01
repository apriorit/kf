#pragma once
#include <kf/stl/new>
#include <kf/BitmapRangeIterator.h>

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // Bitmap - effectively works with set of bits

    template<POOL_TYPE poolType>
    class Bitmap
    {
    public:
        Bitmap() noexcept = default;

        ~Bitmap() noexcept
        {
            deinitialize();
        }

        // Non-copyable
        Bitmap(const Bitmap&) = delete;
        Bitmap& operator=(const Bitmap&) = delete;

        // Movable
        Bitmap(Bitmap&& other) noexcept : m_header(other.m_header)
        {
            other.m_header = {};
        }

        Bitmap& operator=(Bitmap&& other) noexcept
        {
            if (&other != this)
            {
                deinitialize();
                m_header = other.m_header;
                other.m_header = {};
            }

            return *this;
        }

        // IRQL <= APC_LEVEL for PagedPool and on Windows 7 and earlier
        [[nodiscard]] NTSTATUS initialize(ULONG size) noexcept
        {
            deinitialize();

            // Convert bits to bytes and align up to sizeof(ULONG) bytes
            const ULONG bufferSize = ALIGN_UP_BY(size, sizeof(ULONG) * CHAR_BIT) / CHAR_BIT;

            auto buffer = operator new(bufferSize, poolType);
            if (!buffer)
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            RtlZeroMemory(buffer, bufferSize);
            RtlInitializeBitMap(&m_header, static_cast<PULONG>(buffer), size);

            return STATUS_SUCCESS;
        }

        ULONG size() const
        {
            return m_header.SizeOfBitMap;
        }

        void setBits(ULONG startingIndex, ULONG numberToSet) noexcept
        {
            RtlSetBits(&m_header, startingIndex, numberToSet);
        }

        void clearBits(ULONG startingIndex, ULONG numberToSet) noexcept
        {
            RtlClearBits(&m_header, startingIndex, numberToSet);
        }

        void setAll() noexcept
        {
            RtlSetAllBits(&m_header);
        }

        void clearAll() noexcept
        {
            RtlClearAllBits(&m_header);
        }

        bool areBitsSet(ULONG startingIndex, ULONG size) noexcept
        {
            return RtlAreBitsSet(&m_header, startingIndex, size);
        }

        bool areBitsClear(ULONG startingIndex, ULONG size) noexcept
        {
            return RtlAreBitsClear(&m_header, startingIndex, size);
        }

        ULONG numberOfSetBits() noexcept
        {
            return RtlNumberOfSetBits(&m_header);
        }

        ULONG numberOfClearBits() noexcept
        {
            return RtlNumberOfClearBits(&m_header);
        }

        BitmapRangeIterator rangeIterator(ULONG startingIndex = 0) noexcept
        {
            return BitmapRangeIterator(&m_header, startingIndex);
        }

    private:
        void deinitialize() noexcept
        {
            operator delete(m_header.Buffer);
            m_header = {};
        }

    private:
        RTL_BITMAP m_header = {};
    };
}
