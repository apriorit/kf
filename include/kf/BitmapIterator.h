#pragma once

namespace kf
{
    class BitmapRangeIterator
    {
    public:
        BitmapRangeIterator(PRTL_BITMAP header, ULONG startingIndex = 0)
            : m_index(startingIndex)
            , m_header(header)
        {
        }

        struct Range
        {
            ULONG startingIndex;
            ULONG size;
        };

        Range next()
        {
            ULONG startingIndex = RtlFindSetBits(m_header, 1, m_index);
            // For the case if Bitmap doesn't contain set bits after m_index, but contains some set bits ranges,
            // it can return the starting index of the first set range in the bitmap that will be lower than m_index
            if (ULONG_MAX == startingIndex || m_index > startingIndex)
            {
                return {};
            }

            ULONG clearBitsStart = RtlFindClearBits(m_header, 1, startingIndex);

            m_index = clearBitsStart;
            return { startingIndex, clearBitsStart - startingIndex };
        }

    private:
        ULONG m_index = 0;
        PRTL_BITMAP m_header;
    };
}
