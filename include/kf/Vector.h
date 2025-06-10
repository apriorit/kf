// Requires the module that uses kf::vector to define the following methods:
//   _CrtDbgReport
//   _Xinvalid_argument
//   _Xlength_error
//   _Xout_of_range

#pragma once

#include "Allocator.h"
#include <vector>
#include <optional>

#if _KERNEL_MODE && _ITERATOR_DEBUG_LEVEL > 0
#error "_ITERATOR_DEBUG_LEVEL must not be greater than 0"
#endif

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // vector

    template<class T, POOL_TYPE PoolType, class Allocator = Allocator<T, PoolType>>
    class vector
    {
    public:
        using size_type = Allocator::size_type;
        using vector_type = std::vector<T, Allocator>;
        using iterator = vector_type::iterator;
        using const_iterator = vector_type::const_iterator;
        using reverse_iterator = vector_type::reverse_iterator;
        using const_reverse_iterator = vector_type::const_reverse_iterator;

        //
        // Member functions
        //
        constexpr vector() noexcept = default;

        vector(const vector&) = delete;
        vector& operator=(const vector&) = delete;

        constexpr vector(vector&& other) noexcept = default;
        constexpr vector& operator=(vector&& other) noexcept = default;

        constexpr NTSTATUS assign(size_type count, const T& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(count);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            m_vector.assign(count, value);

            return STATUS_SUCCESS;
        }

        constexpr Allocator get_allocator() noexcept
        {
            return m_vector.get_allocator();
        }

        //
        // Element access
        //
        constexpr std::optional<std::reference_wrapper<T>> at(size_type pos) noexcept
        {
            return pos < m_vector.size() ? std::optional(std::ref(m_vector[pos])) : std::nullopt;
        }

        constexpr std::optional<std::reference_wrapper<const T>> at(size_type pos) const noexcept
        {
            return pos < m_vector.size() ? std::optional(std::ref(m_vector[pos])) : std::nullopt;
        }

        constexpr T& operator[](size_type pos) noexcept
        {
            return m_vector[pos];
        }

        constexpr const T& operator[](size_type pos) const noexcept
        {
            return m_vector[pos];
        }

        constexpr T& front() noexcept
        {
            return m_vector.front();
        }

        constexpr const T& front() const noexcept
        {
            return m_vector.front();
        }

        constexpr T& back() noexcept
        {
            return m_vector.back();
        }

        constexpr const T& back() const noexcept
        {
            return m_vector.back();
        }

        constexpr T* data()
        {
            return m_vector.data();
        }

        constexpr const T* data() const noexcept
        {
            return m_vector.data();
        }

        //
        // Iterators
        //

        constexpr iterator begin() noexcept
        {
            return m_vector.begin();
        }

        constexpr const_iterator begin() const noexcept
        {
            return m_vector.begin();
        }

        constexpr const_iterator cbegin() const noexcept
        {
            return m_vector.cbegin();
        }

        constexpr iterator end() noexcept
        {
            return m_vector.end();
        }

        constexpr const_iterator end() const noexcept
        {
            return m_vector.end();
        }

        constexpr const_iterator cend() const noexcept
        {
            return m_vector.cend();
        }

        constexpr reverse_iterator rbegin() noexcept
        {
            return m_vector.rbegin();
        }

        constexpr const_reverse_iterator rbegin() const noexcept
        {
            return m_vector.rbegin();
        }

        constexpr const_reverse_iterator crbegin() const noexcept
        {
            return m_vector.crbegin();
        }

        constexpr reverse_iterator rend() noexcept
        {
            return m_vector.rend();
        }

        constexpr const_reverse_iterator rend() const noexcept
        {
            return m_vector.rend();
        }

        constexpr const_reverse_iterator crend() const noexcept
        {
            return m_vector.crend();
        }

        //
        // Capacity
        //

        constexpr bool empty() const noexcept
        {
            return m_vector.empty();
        }

        constexpr size_type size() const noexcept
        {
            return m_vector.size();
        }

        constexpr size_type max_size() const noexcept
        {
            return m_vector.max_size();
        }

        constexpr NTSTATUS reserve(size_type newCapacity) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            if (newCapacity <= m_vector.capacity())
            {
                return STATUS_SUCCESS;
            }

            return reallocateExactly(newCapacity);
        }

        constexpr size_type capacity() const noexcept
        {
            return m_vector.capacity();
        }

        constexpr NTSTATUS shrink_to_fit() noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            if (m_vector.size() == m_vector.capacity())
            {
                return STATUS_SUCCESS;
            }

            auto status = reallocateExactly(m_vector.size());
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            m_vector.shrink_to_fit();

            return STATUS_SUCCESS;
        }

        //
        // Modifiers
        //

        constexpr void clear() noexcept
        {
            m_vector.clear();
        }

        constexpr std::optional<iterator> insert(const_iterator pos, const T& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(m_vector.size() + 1);
            if (!NT_SUCCESS(status))
            {
                return std::nullopt;
            }

            return m_vector.insert(pos, value);
        }

        constexpr std::optional<iterator> insert(const_iterator pos, T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(m_vector.size() + 1);
            if (!NT_SUCCESS(status))
            {
                return std::nullopt;
            }

            return m_vector.insert(pos, std::move(value));
        }

        constexpr std::optional<iterator> insert(const_iterator pos, size_type count, const T& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(m_vector.size() + count);
            if (!NT_SUCCESS(status))
            {
                return std::nullopt;
            }

            return m_vector.insert(pos, count, value);
        }

        template<class... Args>
        constexpr std::optional<iterator> emplace(const_iterator pos, Args&&... args) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(m_vector.size() + 1);
            if (!NT_SUCCESS(status))
            {
                return std::nullopt;
            }

            return m_vector.emplace(pos, std::forward<Args>(args)...);
        }

        constexpr iterator erase(const_iterator pos) noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            return m_vector.erase(pos);
        }

        constexpr iterator erase(const_iterator first, const_iterator last) noexcept(std::is_nothrow_move_assignable_v<T>)
        {
            return m_vector.erase(first, last);
        }

        constexpr NTSTATUS push_back(const T& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(m_vector.size() + 1);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            m_vector.push_back(value);

            return STATUS_SUCCESS;
        }

        constexpr NTSTATUS push_back(T&& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(m_vector.size() + 1);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            m_vector.push_back(std::move(value));

            return STATUS_SUCCESS;
        }

        template<class... Args>
        constexpr std::optional<std::reference_wrapper<T>> emplace_back(Args&&... args) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(m_vector.size() + 1);
            if (!NT_SUCCESS(status))
            {
                return std::nullopt;
            }

            return m_vector.emplace_back(std::forward<Args>(args)...);
        }

        constexpr void pop_back() noexcept
        {
            m_vector.pop_back();
        }

        constexpr NTSTATUS resize(size_type count) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(count);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            m_vector.resize(count);

            return STATUS_SUCCESS;
        }

        constexpr NTSTATUS resize(size_type count, const T& value) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            auto status = reallocateGrowth(count);
            if (!NT_SUCCESS(status))
            {
                return status;
            }

            m_vector.resize(count, value);

            return STATUS_SUCCESS;
        }

        constexpr void swap(vector& other) noexcept(std::is_nothrow_swappable_v<std::vector<T, Allocator>>)
        {
            m_vector.swap(other.m_vector);
        }

    private:
        constexpr NTSTATUS reallocateGrowth(size_type newSize) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            if (newSize <= m_vector.capacity())
            {
                return STATUS_SUCCESS;
            }

            return moveInternal(calculateGrowth(newSize));
        }

        constexpr NTSTATUS reallocateExactly(size_type newSize) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            return moveInternal(newSize);
        }

        constexpr NTSTATUS moveInternal(size_type newCapacity) noexcept(std::is_nothrow_move_constructible_v<T>)
        {
            std::vector<T, Allocator> newVector;
            newVector.reserve(newCapacity);
            if (!newVector.data())
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            for (auto& elem : m_vector)
            {
                newVector.emplace_back(std::move(elem));
            }

            m_vector.swap(newVector);

            return STATUS_SUCCESS;
        }

        constexpr size_type calculateGrowth(size_type required) noexcept
        {
            const auto oldCapacity = capacity();
            const auto max = max_size();

            if (oldCapacity > max - oldCapacity / 2)
            {
                return max; // geometric growth would overflow
            }

            const auto geometric = oldCapacity + oldCapacity / 2;

            if (geometric < required)
            {
                return required; // geometric growth would be insufficient
            }

            return geometric; // geometric growth is sufficient
        }

    private:
        std::vector<T, Allocator> m_vector;
    };
}
