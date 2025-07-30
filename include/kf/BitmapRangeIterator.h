#pragma once
#include <optional>
#include <kf/stl/cassert>

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // BitmapRangeIterator - enumerates ranges of bits set to 1

    class BitmapRangeIterator
    {
    public:
        BitmapRangeIterator(PRTL_BITMAP header, ULONG startingIndex = 0)
            : m_header(header)
            , m_index(startingIndex)
        {
        }

        //
        // Return range of set bits as a pair { startIndex, lenght }, nullopt means the end of iteration
        //

        std::optional<std::pair<ULONG, ULONG>> next()
        {
            const auto range = internalNext();

            return range.second ? std::make_optional(range) : std::nullopt;
        }

    private:
        std::pair<ULONG, ULONG> internalNext()
        {
            if (m_index >= m_header->SizeOfBitMap)
            {
                //
                // We're beyond the end, there is nothing to report
                //

                return {};
            }

            ULONG clearBitsStart = ULONG_MAX;
            ULONG clearBitsCount = RtlFindNextForwardRunClear(m_header, m_index, &clearBitsStart);

            if (!clearBitsCount)
            {
                //
                // No clear bits are found, then we have the range at the end of the bitmap to report.
                // Set `clearBitsStart` beyond the last bit.
                //

                clearBitsStart = m_header->SizeOfBitMap;
            }

            //
            // Range of set bits is from the current index to the first clear bit
            //

            std::pair<ULONG, ULONG> range = { m_index, clearBitsStart - m_index };

            //
            // Update the current index to the next bit beyond the last clear bit we've found
            //

            m_index = clearBitsStart + clearBitsCount;

            if (!range.second)
            {
                //
                // We've got an empty range! The bitmap starts from clear bits so we need to run internalNext again
                //

                assert(!range.first); // We expect this case only at the beginning of the bitmap

                range = internalNext();
            }

            return range;
        }

    private:
        PRTL_BITMAP m_header;
        ULONG       m_index = 0;
    };
}
