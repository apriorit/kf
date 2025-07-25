#pragma once
#include "stl/new"

namespace kf
{
    template<typename T, POOL_TYPE _Pool = PagedPool>
    class scoped_buffer
    {
    public:
        scoped_buffer()
            : m_size(0)
        {
            m_buf = nullptr;
        }

        ~scoped_buffer()
        {
            clear();
        }

        scoped_buffer(__in const scoped_buffer&) = delete;
        scoped_buffer& operator=(__in const scoped_buffer&) = delete;

        void operator==(__in const scoped_buffer&) const = delete;
        void operator!=(__in const scoped_buffer&) const = delete;

        scoped_buffer(__inout scoped_buffer&& other)
            : m_buf(other.m_buf)
            , m_size(other.m_size)
        {
            other.m_buf = nullptr;
            other.m_size = 0;
        }

        scoped_buffer& operator=(__inout scoped_buffer&& other)
        {
            if (this == &other)
            {
                return *this;
            }
            clear();
            m_buf = other.m_buf;
            m_size = other.m_size;
            other.m_buf = nullptr;
            other.m_size = 0;
            return *this;
        }

        NTSTATUS resize(__in ULONG size)
        {
            T* buf = new(_Pool) T[size];

            if (!buf)
            {
                return STATUS_NO_MEMORY;
            }

            if (m_buf)
            {
                const size_t minsize = min(m_size, size);
                RtlCopyMemory(buf, m_buf, minsize * sizeof(T));
                clear();
            }

            m_buf = buf;
            m_size = size;

            return STATUS_SUCCESS;
        }

        ULONG size() const
        {
            return m_size;
        }

        void clear()
        {
            if (m_buf)
            {
                delete[] m_buf;
                m_buf = nullptr;
                m_size = 0;
            }
            ASSERT(m_size == 0);
        }

        T* release()
        {
            auto ret = m_buf;

            m_buf = nullptr;
            m_size = 0;

            return ret;
        }

        T* get()
        {
            return m_buf;
        }

        const T* get() const
        {
            return m_buf;
        }

        T& operator*() const
        {
            return *get();
        }

        T* operator->() const
        {
            return get();
        }

        operator bool() const
        {
            return m_size;
        }

        void swap(__inout scoped_buffer& other)
        {
            T* tmp_buf = m_buf;
            ULONG tmp_size = m_size;

            m_buf = other.m_buf;
            m_size = other.m_size;

            other.m_buf = tmp_buf;
            other.m_size = tmp_size;
        }

        T* begin()
        {
            return get();
        }

        T* end()
        {
            return get() + m_size;
        }

    private:
        T* m_buf;
        ULONG m_size;
    };
}
