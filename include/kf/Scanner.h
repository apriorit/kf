#pragma once
#include "USimpleString.h"
#include <span>
#include "SpanUtils.h"

namespace kf
{

    class Scanner
    {
    public:
        Scanner(span<const std::byte> data) : m_data(data)
        {
        }

        template<class T>
        bool hasNext() const
        {
            return m_data.size() / sizeof(T) > 0;
        }

        template<class T>
        T next()
        {
            auto elem = reinterpret_cast<const T*>(m_data.begin());
            
            skip(sizeof(T));
            
            return *elem;
        }

        bool hasNextLineA() const
        {
            return m_data.size() / sizeof(wchar_t) > 0;
        }

        bool hasNextLineW() const
        {
            return m_data.size() / sizeof(char) > 0;
        }

        USimpleString nextLineW()
        {
            auto data = span_cast<const wchar_t>(m_data);

            ptrdiff_t fromIndex = 0;
            const auto str = split(data, L'\n', fromIndex);

            skip(static_cast<int>(fromIndex > 0 ? fromIndex * sizeof(wchar_t) : m_data.size()));

            return USimpleString(str).trimRight(L'\r');
        }

        ASimpleString nextLineA()
        {
            auto data = span_cast<const char>(m_data);

            ptrdiff_t fromIndex = 0;
            const auto str = split(data, '\n', fromIndex);

            skip(static_cast<int>(fromIndex > 0 ? fromIndex * sizeof(char) : m_data.size()));

            return ASimpleString(str).trimRight('\r');
        }

        void skip(int bytes)
        {
            m_data = m_data.subspan(bytes);
        }

    private:
        span<const std::byte> m_data;
    };
}
