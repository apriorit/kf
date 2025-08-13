#pragma once
#include "kf/stl/new"
/*
 * Taken from https://github.com/swenson/sort
 * Revision: 05fd77bfec049ce8b7c408c4d3dd2d51ee061a15
 * Removed all code unrelated to Timsort and made minor adjustments for
 * cross-platform compatibility.
 */

/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2010-2017 Christopher Swenson.
 * Copyright (c) 2012 Vojtech Fried.
 * Copyright (c) 2012 Google Inc. All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <algorithm>
#include <cstdint>

namespace timsort
{
    namespace detail
    {
        using namespace std;

        template<class T>
        inline int cmp(const T& lhs, const T& rhs)
        {
            return lhs < rhs ? -1 : (rhs < lhs ? 1 : 0);
        }

        const int kStackSize = 128;

        /* adapted from Hacker's Delight */
        inline int clzll(uint64_t x)
        {
            if (x == 0)
            {
                return 64;
            }

            int n = 0;

            if (x <= 0x00000000FFFFFFFFL)
            {
                n = n + 32;
                x = x << 32;
            }

            if (x <= 0x0000FFFFFFFFFFFFL)
            {
                n = n + 16;
                x = x << 16;
            }

            if (x <= 0x00FFFFFFFFFFFFFFL)
            {
                n = n + 8;
                x = x << 8;
            }

            if (x <= 0x0FFFFFFFFFFFFFFFL)
            {
                n = n + 4;
                x = x << 4;
            }

            if (x <= 0x3FFFFFFFFFFFFFFFL)
            {
                n = n + 2;
                x = x << 2;
            }

            if (x <= 0x7FFFFFFFFFFFFFFFL)
            {
                n = n + 1;
            }

            return n;
        }

        inline int compute_minrun(const uint64_t size)
        {
            const int top_bit = 64 - clzll(size);
            const int shift = max(top_bit, 6) - 6;
            const int minrun = static_cast<int>(size >> shift);
            const uint64_t mask = (1ULL << shift) - 1;

            if (mask & size)
            {
                return minrun + 1;
            }

            return minrun;
        }

        inline void free(void* p)
        {
            delete p;
        }

        inline void* malloc(size_t size)
        {
            return new(PagedPool) uint8_t[size];
        }

        struct TIM_SORT_RUN_T
        {
            size_t start;
            size_t length;
        };

        /* Function used to do a binary search for binary insertion sort */
        template<class T>
        inline size_t binary_insertion_find(T* dst, const T& x, const size_t size)
        {
            size_t l = 0;
            size_t r = size - 1;
            size_t c = r >> 1;

            if (dst == nullptr || size == 0)
            {
                return 0;
            }

            /* check for out of bounds at the beginning. */
            if (cmp(x, dst[0]) < 0)
            {
                return 0;
            }
            else if (cmp(x, dst[r]) >= 0)
            {
                return size;
            }

            T* cx = &dst[c];

            while (true)
            {
                const int val = cmp(x, *cx);

                if (val < 0)
                {
                    if (c - l <= 1)
                    {
                        return c;
                    }

                    r = c;
                }
                else /* allow = for stability. The binary search favors the right. */
                {
                    if (r - c <= 1)
                    {
                        return c + 1;
                    }

                    l = c;
                }

                c = l + ((r - l) >> 1);
                cx = &dst[c];
            }
        }

        /* Binary insertion sort, but knowing that the first "start" entries are sorted.  Used in timsort. */
        template<class T>
        inline void binary_insertion_sort_start(T* dst, const size_t start, const size_t size)
        {
            if (dst == nullptr || size == 0)
            {
                return;
            }

            for (size_t i = start; i < size; i++)
            {
                /* If this entry is already correct, just move along */
                if (cmp(dst[i - 1], dst[i]) <= 0)
                {
                    continue;
                }

                /* Else we need to find the right place, shift everything over, and squeeze in */
                T x = dst[i];
                size_t location = binary_insertion_find(dst, x, i);

                for (size_t j = i - 1; j >= location; j--)
                {
                    dst[j + 1] = dst[j];

                    if (j == 0) /* check edge case because j is unsigned */
                    {
                        break;
                    }
                }

                dst[location] = x;
            }
        }

        /* timsort implementation, based on timsort.txt */
        template<class T>
        inline void reverse_elements(T* dst, size_t start, size_t end)
        {
            if (dst == nullptr)
            {
                return;
            }

            while (true)
            {
                if (start >= end)
                {
                    return;
                }

                swap(dst[start], dst[end]);
                start++;
                end--;
            }
        }

        template<class T>
        inline size_t count_run(T* dst, const size_t start, const size_t size)
        {
            if (dst == nullptr || size == 0)
            {
                return 0;
            }

            size_t curr;

            if (size - start == 1)
            {
                return 1;
            }

            if (start >= size - 2)
            {
                if (cmp(dst[size - 2], dst[size - 1]) > 0)
                {
                    swap(dst[size - 2], dst[size - 1]);
                }

                return 2;
            }

            curr = start + 2;

            if (cmp(dst[start], dst[start + 1]) <= 0)
            {
                /* increasing run */
                while (true)
                {
                    if (curr == size)
                    {
                        break;
                    }

                    if (cmp(dst[curr - 1], dst[curr]) > 0)
                    {
                        break;
                    }

                    curr++;
                }

                return curr - start;
            }
            else
            {
                /* decreasing run */
                while (true)
                {
                    if (curr == size)
                    {
                        break;
                    }

                    if (cmp(dst[curr - 1], dst[curr]) <= 0)
                    {
                        break;
                    }

                    curr++;
                }

                /* reverse in-place */
                reverse_elements(dst, start, curr - 1);
                return curr - start;
            }
        }

        inline int check_invariant(TIM_SORT_RUN_T* stack, const int stack_curr)
        {
            if (stack == nullptr)
            {
                return -1;
            }

            if (stack_curr < 2)
            {
                return 1;
            }

            if (stack_curr == 2)
            {
                const size_t A1 = stack[stack_curr - 2].length;
                const size_t B1 = stack[stack_curr - 1].length;

                if (A1 <= B1)
                {
                    return 0;
                }

                return 1;
            }

            size_t A = stack[stack_curr - 3].length;
            size_t B = stack[stack_curr - 2].length;
            size_t C = stack[stack_curr - 1].length;

            if ((A <= B + C) || (B <= C))
            {
                return 0;
            }

            return 1;
        }

        template<class T>
        struct TEMP_STORAGE_T
        {
            size_t alloc = 0;
            T* storage = nullptr;
        };

        template<class T>
        inline void tim_sort_resize(TEMP_STORAGE_T<T>* store, const size_t new_size)
        {
            if (store == nullptr)
            {
                return;
            }

            if (store->alloc < new_size)
            {
                T* tempstore = (T*)malloc(new_size * sizeof(T));
                if (tempstore == nullptr)
                {
                    return;
                }

                if (store->storage != nullptr)
                {
                    memcpy(tempstore, store->storage, store->alloc * sizeof(T));
                    detail::free(store->storage);
                }

                store->storage = tempstore;
                store->alloc = new_size;
            }
        }

        template<class T>
        inline void tim_sort_merge(T* dst, const TIM_SORT_RUN_T* stack, const int stack_curr, TEMP_STORAGE_T<T>* store)
        {
            const size_t A = stack[stack_curr - 2].length;
            const size_t B = stack[stack_curr - 1].length;
            const size_t curr = stack[stack_curr - 2].start;
            size_t i, j, k;
            tim_sort_resize(store, min(A, B));
            T* storage = store->storage;

            /* left merge */
            if (A < B)
            {
                memcpy(storage, &dst[curr], A * sizeof(T));
                i = 0;
                j = curr + A;

                for (k = curr; k < curr + A + B; k++)
                {
                    if ((i < A) && (j < curr + A + B))
                    {
                        if (cmp(storage[i], dst[j]) <= 0)
                        {
                            dst[k] = storage[i++];
                        }
                        else
                        {
                            dst[k] = dst[j++];
                        }
                    }
                    else if (i < A)
                    {
                        dst[k] = storage[i++];
                    }
                    else
                    {
                        break;
                    }
                }
            }
            else
            {
                /* right merge */
                memcpy(storage, &dst[curr + A], B * sizeof(T));
                i = B;
                j = curr + A;
                k = curr + A + B;

                while (k > curr)
                {
                    k--;
                    if ((i > 0) && (j > curr))
                    {
                        if (cmp(dst[j - 1], storage[i - 1]) > 0)
                        {
                            dst[k] = dst[--j];
                        }
                        else
                        {
                            dst[k] = storage[--i];
                        }
                    }
                    else if (i > 0)
                    {
                        dst[k] = storage[--i];
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }

        template<class T>
        inline int tim_sort_collapse(T* dst, TIM_SORT_RUN_T* stack, int stack_size, TEMP_STORAGE_T<T>* store, const size_t size)
        {
            while (true)
            {
                /* if the stack only has one thing on it, we are done with the collapse */
                if (stack_size <= 1)
                {
                    break;
                }

                /* if this is the last merge, just do it */
                if ((stack_size == 2) && (stack[0].length + stack[1].length == size))
                {
                    tim_sort_merge(dst, stack, stack_size, store);
                    stack[0].length += stack[1].length;
                    stack[1] = { 0, 0 };
                    stack_size--;
                    break;
                }
                /* check if the invariant is off for a stack of 2 elements */
                else if ((stack_size == 2) && (stack[0].length <= stack[1].length))
                {
                    tim_sort_merge(dst, stack, stack_size, store);
                    stack[0].length += stack[1].length;
                    stack[1] = { 0, 0 };
                    stack_size--;
                    break;
                }
                else if (stack_size == 2)
                {
                    break;
                }

                size_t A;
                size_t B = stack[stack_size - 3].length;
                size_t C = stack[stack_size - 2].length;
                size_t D = stack[stack_size - 1].length;

                int ABC, BCD, CD;

                if (stack_size >= 4)
                {
                    A = stack[stack_size - 4].length;
                    ABC = (A <= B + C);
                }
                else
                {
                    ABC = 0;
                }

                BCD = (B <= C + D) || ABC;
                CD = (C <= D);

                /* Both invariants are good */
                if (!BCD && !CD)
                {
                    break;
                }

                /* left merge */
                if (BCD && !CD)
                {
                    tim_sort_merge(dst, stack, stack_size - 1, store);
                    stack[stack_size - 3].length += stack[stack_size - 2].length;
                    stack[stack_size - 2] = stack[stack_size - 1];
                    stack[stack_size - 1] = { 0, 0 };
                    stack_size--;
                }
                else
                {
                    /* right merge */
                    tim_sort_merge(dst, stack, stack_size, store);
                    stack[stack_size - 2].length += stack[stack_size - 1].length;
                    stack[stack_size - 1] = { 0, 0 };
                    stack_size--;
                }
            }

            return stack_size;
        }

        template<class T>
        inline int PUSH_NEXT(T* dst,
            const size_t size,
            TEMP_STORAGE_T<T>* store,
            const size_t minrun,
            TIM_SORT_RUN_T* run_stack,
            size_t* stack_size,
            size_t* curr)
        {

            if (dst == nullptr ||
                store == nullptr ||
                run_stack == nullptr ||
                stack_size == nullptr ||
                curr == nullptr)
            {
                return 0;
            }

            size_t len = count_run(dst, *curr, size);
            size_t run = minrun;

            /* If there is less than minrun left until the end of the array */
            if (run > size - *curr)
            {
                run = size - *curr;
            }

            /* If the found sorted run is smaller than minrun,
             * we need to extend this segment by sorting the rest of the elements to match the run size */
            if (run > len)
            {
                binary_insertion_sort_start(&dst[*curr], len, run);
                len = run;
            }

            run_stack[*stack_size].start = *curr;
            run_stack[*stack_size].length = len;
            (*stack_size)++;
            *curr += len;

            if (*curr == size)
            {
                /* finish up */
                while (*stack_size > 1)
                {
                    tim_sort_merge(dst, run_stack, static_cast<int>(*stack_size), store);
                    run_stack[*stack_size - 2].length += run_stack[*stack_size - 1].length;
                    (*stack_size)--;
                }

                if (store->storage)
                {
                    detail::free(store->storage);
                    store->storage = nullptr;
                }

                return 0;
            }

            return 1;
        }
    }

    /* Binary insertion sort */
    template<class T>
    inline void binary_insertion_sort(T* dst, const size_t size)
    {
        if (dst == nullptr)
        {
            return;
        }

        /* don't bother sorting an array of size <= 1 */
        if (size <= 1)
        {
            return;
        }

        detail::binary_insertion_sort_start(dst, 1, size);
    }

    template<class T>
    inline void tim_sort(T* dst, const size_t size)
    {
        if (dst == nullptr)
        {
            return;
        }

        /* don't bother sorting an array of size 1 */
        if (size <= 1)
        {
            return;
        }

        if (size < 64)
        {
            binary_insertion_sort(dst, size);
            return;
        }

        /* compute the minimum run length */
        size_t minrun = detail::compute_minrun(size);

        /* temporary storage for merges */
        detail::TEMP_STORAGE_T<T> store;

        detail::TIM_SORT_RUN_T run_stack[detail::kStackSize];
        size_t stack_curr = 0;
        size_t curr = 0;

        if (!detail::PUSH_NEXT(dst, size, &store, minrun, run_stack, &stack_curr, &curr))
        {
            return;
        }

        if (!detail::PUSH_NEXT(dst, size, &store, minrun, run_stack, &stack_curr, &curr))
        {
            return;
        }

        if (!detail::PUSH_NEXT(dst, size, &store, minrun, run_stack, &stack_curr, &curr))
        {
            return;
        }

        while (true)
        {
            if (!detail::check_invariant(run_stack, static_cast<int>(stack_curr)))
            {
                stack_curr = detail::tim_sort_collapse(dst, run_stack, static_cast<int>(stack_curr), &store, size);
                continue;
            }

            if (!detail::PUSH_NEXT(dst, size, &store, minrun, run_stack, &stack_curr, &curr))
            {
                return;
            }
        }
    }
}
