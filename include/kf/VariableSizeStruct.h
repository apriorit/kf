#pragma once
#include <kf/stl/new>

namespace kf
{
    template<class T, POOL_TYPE poolType>
    class VariableSizeStruct
    {
    public:
        VariableSizeStruct(__in const VariableSizeStruct&) = delete;
        VariableSizeStruct& operator=(const VariableSizeStruct&) = delete;
        VariableSizeStruct() = default;

        template<class... Args>
        VariableSizeStruct(int bytes, Args&&... args)
        {
            emplace(bytes, std::forward<Args>(args)...);
        }

        ~VariableSizeStruct()
        {
            free();
        }

        template<class... Args>
        NTSTATUS emplace(int bytes, Args&&... args)
        {
            free();

            m_buffer = reinterpret_cast<T*>(new(poolType) std::byte[bytes]);
            if (!m_buffer)
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            new(m_buffer) T(std::forward<Args>(args)...);
            return STATUS_SUCCESS;
        }

        void free()
        {
            if (m_buffer)
            {
                get()->~T();
                delete[] reinterpret_cast<std::byte*>(m_buffer);
                m_buffer = nullptr;
            }
        }

        T* get()
        {
            return m_buffer;
        }

        const T* get() const
        {
            return m_buffer;
        }

        operator T* ()
        {
            return get();
        }

        T* operator->()
        {
            return get();
        }

        const T* operator->() const
        {
            return get();
        }

    private:
        T* m_buffer = nullptr;
    };
}
