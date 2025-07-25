#pragma once

namespace kf
{
    namespace Guard
    {
        //////////////////////////////////////////////////////////////////////////
        // Guard

#define GUARD_TYPE(valueType, invalidValue, closeFunction) kf::Guard::Guard<valueType, invalidValue, decltype(closeFunction), closeFunction>

        template<class TValueType, TValueType invalidValue, class TCloseFunctionType, TCloseFunctionType closeFunction, typename TCloseValueType = TValueType>
        class Guard
        {
        public:
            Guard() : m_value(invalidValue)
            {
            }

            explicit Guard(TValueType value) : m_value(value)
            {
            }

            Guard(Guard&& another) : m_value(another.release()) 
            { 
            }

            ~Guard() 
            { 
                reset();
            }

            Guard& operator=(Guard&& another)
            {
                if (this != &another)
                {
                    reset(another.release());
                }

                return *this;
            }

            operator TValueType() const 
            { 
                return m_value; 
            }

            TValueType operator->() const 
            { 
                return m_value; 
            }

            TValueType get() const 
            { 
                return m_value; 
            }

            TValueType& get() 
            { 
                return m_value; 
            }

            void reset(TValueType value = invalidValue)
            {
                if (m_value != invalidValue)
                {
                    closeFunction(reinterpret_cast<TCloseValueType>(m_value));
                }

                m_value = value;
            }

            TValueType release() 
            { 
                TValueType value = m_value;  // Release ownership

                m_value = invalidValue;

                return value;
            }

        private:
            Guard(const Guard&);
            Guard& operator=(const Guard&);

        private:
            TValueType m_value;
        };

        //////////////////////////////////////////////////////////////////////////
        // Guard typedefs    

#ifdef _WDFTYPES_H_
        typedef Guard<::WDFREQUEST, nullptr, void (*)(::WDFOBJECT), &::WdfObjectDelete, ::WDFOBJECT>    WdfRequest;
        typedef GUARD_TYPE(::WDFKEY, nullptr, &::WdfRegistryClose)                                      WdfKey;
#endif

#ifdef __FLTKERNEL__
        typedef GUARD_TYPE(::PFLT_FILE_NAME_INFORMATION, nullptr, &::FltReleaseFileNameInformation)     FltFileNameInformation;
        typedef GUARD_TYPE(::HANDLE, nullptr, &::FltClose)                                              FltFileHandle;
        typedef GUARD_TYPE(::PFLT_VOLUME, nullptr, &::FltObjectDereference)                             FltVolume;
        typedef GUARD_TYPE(::PFLT_INSTANCE, nullptr, &::FltObjectDereference)                           FltInstance;
        typedef GUARD_TYPE(::PFLT_FILTER, nullptr, &::FltObjectDereference)                             FltFilter;
#endif

        typedef GUARD_TYPE(::PFILE_OBJECT, nullptr, &::ObfDereferenceObject)                            FileObject;
        typedef GUARD_TYPE(::PFILE_OBJECT, nullptr, &::ObDereferenceObjectDeferDelete)                  DeferFileObject;
        typedef GUARD_TYPE(::PDEVICE_OBJECT, nullptr, &::ObfDereferenceObject)                          DeviceObject;
        typedef GUARD_TYPE(::PDRIVER_OBJECT, nullptr, &::ObfDereferenceObject)                          DriverObject;
        typedef GUARD_TYPE(::PEPROCESS, nullptr, &::ObfDereferenceObject)                               EProcess;
        typedef GUARD_TYPE(::PETHREAD, nullptr, &::ObfDereferenceObject)                                EThread;
        typedef GUARD_TYPE(::HANDLE, nullptr, &::ZwClose)                                               Handle;
        typedef GUARD_TYPE(::PACCESS_TOKEN, nullptr, &::ObfDereferenceObject)                           AccessToken;
        typedef GUARD_TYPE(::PVOID, nullptr, &::ObfDereferenceObject)                                   Object;
    }
}
