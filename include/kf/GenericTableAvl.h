#pragma once
#include <functional>
#include <utility>
#include <kf/stl/new>

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // GenercTableAvl

    template<class T, POOL_TYPE poolType, class LessComparer=std::less<T>>
    class GenericTableAvl
    {
    public:
        GenericTableAvl(const GenericTableAvl&) = delete;
        GenericTableAvl& operator=(const GenericTableAvl&) = delete;

        GenericTableAvl()
        {
            init();
        }

        GenericTableAvl(_Inout_ GenericTableAvl&& another)
        {
            moveInit(another);
        }

        ~GenericTableAvl()
        {
            clear();
        }

        NTSTATUS insertElement(_Inout_ T&& elem, _Out_opt_ bool* newElement = nullptr)
        {
            BOOLEAN newElementBoolean = false;

            void* buffer = ::RtlInsertElementGenericTableAvl(&m_table, &elem, sizeof(elem), &newElementBoolean);
            if (!buffer)
            {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            if (!newElementBoolean)
            {
                static_cast<T*>(buffer)->~T();
            }

            new(buffer) T(std::move(elem));

            if (newElement)
            {
                *newElement = !!newElementBoolean;
            }

            return STATUS_SUCCESS;
        }

        bool isEmpty() const
        {
            return !!::RtlIsGenericTableEmptyAvl(const_cast<RTL_AVL_TABLE*>(&m_table));
        }

        T* lookupElement(_In_ const T& elem)
        {
            return static_cast<T*>(::RtlLookupElementGenericTableAvl(&m_table, const_cast<T*>(&elem)));
        }

        const T* lookupElement(_In_ const T& elem) const
        {
            return const_cast<GenericTableAvl*>(this)->lookupElement(elem);
        }

        T* getElement(_In_ ULONG index)
        {
            return static_cast<T*>(::RtlGetElementGenericTableAvl(&m_table, index));
        }

        bool deleteElement(_In_ const T& elem)
        {
            return !!::RtlDeleteElementGenericTableAvl(&m_table, const_cast<T*>(&elem));
        }

        ULONG number() const
        {
            return ::RtlNumberGenericTableElementsAvl(const_cast<RTL_AVL_TABLE*>(&m_table));
        }

        T* enumerateWithoutSplaying(_Inout_ void*& restartKey)
        {
            return static_cast<T*>(::RtlEnumerateGenericTableWithoutSplayingAvl(&m_table, &restartKey));
        }

        void clear()
        {
            for(;;)
            {
                void* restartKey = nullptr;
                T* elem = enumerateWithoutSplaying(restartKey);
                if (!elem)
                {
                    break;
                }

                deleteElement(*elem);
            }
        }

        GenericTableAvl& operator=(_Inout_ GenericTableAvl&& another)
        {
            if (this != &another)
            {
                clear();
                moveInit(another);
            }

            return *this;
        }

    private:
        void init()
        {
#pragma warning(suppress: 28023) // missing _Function_class_ annotation
            ::RtlInitializeGenericTableAvl(&m_table, &compareRoutine, &allocateRoutine, &freeRoutine, this);
        }

        void moveInit(GenericTableAvl& another)
        {
            m_table = another.m_table;
            m_table.TableContext = this;

            m_table.BalancedRoot.Parent = &m_table.BalancedRoot;

            if (m_table.BalancedRoot.LeftChild)
            {
                m_table.BalancedRoot.LeftChild->Parent = &m_table.BalancedRoot;
            }

            if (m_table.BalancedRoot.RightChild)
            {
                m_table.BalancedRoot.RightChild->Parent = &m_table.BalancedRoot;
            }

            if (m_table.RestartKey == &another.m_table.BalancedRoot)
            {
                m_table.RestartKey = &m_table.BalancedRoot;
            }

            another.init();
        }

        _IRQL_requires_same_
        _Function_class_(RTL_AVL_ALLOCATE_ROUTINE)
        __drv_allocatesMem(Mem)
        static void* NTAPI allocateRoutine(_In_ RTL_AVL_TABLE*, _In_ CLONG byteSize)
        {
            return operator new(byteSize, poolType);
        }

        _IRQL_requires_same_
        _Function_class_(RTL_AVL_FREE_ROUTINE)
        static void NTAPI freeRoutine(_In_ RTL_AVL_TABLE*, _In_ __drv_freesMem(Mem) _Post_invalid_ void* buffer)
        {
            reinterpret_cast<T*>(static_cast<RTL_BALANCED_LINKS*>(buffer) + 1)->~T();
            delete buffer;
        }

        _IRQL_requires_same_
        _Function_class_(RTL_AVL_COMPARE_ROUTINE)
        static RTL_GENERIC_COMPARE_RESULTS NTAPI compareRoutine(_In_ RTL_AVL_TABLE*, _In_ void* first, _In_ void* second)
        {
            const T& firstElem = *static_cast<const T*>(first);
            const T& secondElem = *static_cast<const T*>(second);

            LessComparer lessComparer;

            if (lessComparer(firstElem, secondElem))
            {
                return GenericLessThan;
            }
            else if (lessComparer(secondElem, firstElem))
            {
                return GenericGreaterThan;
            }
            else
            {
                return GenericEqual;
            }
        }

    private:
        enum { PoolTag = '++TG' };

    private:
        RTL_AVL_TABLE m_table;
    };
}
