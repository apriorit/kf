#pragma once
#include "kf/guard/ObjectAttributes.h"
#include "kf/guard/ScopeExit.h"
#include "kf/guard/VariableSizeStruct.h"

namespace kf
{
    //////////////////////////////////////////////////////////////////////////
    // This is a sample handler class for FltCommunicationPort.
    /*
    class SampleHandler
    {
    public:
        static NTSTATUS onConnect(PFLT_FILTER filter, PFLT_PORT clientPort, _In_reads_bytes_opt_(connectionContextLength) PVOID connectionContext, ULONG connectionContextLength, _Outptr_ SampleHandler** handler)
        {
            UNREFERENCED_PARAMETER(connectionContext);
            UNREFERENCED_PARAMETER(connectionContextLength);

            *handler = new(PagedPool) SampleHandler(filter, clientPort);

            return *handler ? STATUS_SUCCESS : STATUS_INSUFF_SERVER_RESOURCES;
        }

        void onDisconnect()
        {
            delete this;
        }

        NTSTATUS onMessage(_In_reads_bytes_opt_(inputBufferLength) PVOID inputBuffer,
            _In_ ULONG inputBufferLength,
            _Out_writes_bytes_to_opt_(outputBufferLength, *returnOutputBufferLength) PVOID outputBuffer,
            _In_ ULONG outputBufferLength,
            _Out_ PULONG returnOutputBufferLength)
        {
            UNREFERENCED_PARAMETER(inputBuffer);
            UNREFERENCED_PARAMETER(inputBufferLength);
            UNREFERENCED_PARAMETER(outputBuffer);
            UNREFERENCED_PARAMETER(outputBufferLength);
            *returnOutputBufferLength = 0;

            return STATUS_SUCCESS;
        }

    private:
        SampleHandler(PFLT_FILTER filter, PFLT_PORT clientPort) : m_filter(filter), m_clientPort(clientPort)
        {
        }

        ~SampleHandler()
        {
            ::FltCloseClientPort(m_filter, &m_clientPort);
        }

    private:
        PFLT_FILTER m_filter;
        PFLT_PORT   m_clientPort;
    };
    */

    extern "C"
    {
        NTSYSAPI NTSTATUS NTAPI RtlSetSaclSecurityDescriptor(PSECURITY_DESCRIPTOR securityDescriptor,
            BOOLEAN saclPresent,
            PACL sacl,
            BOOLEAN saclDefaulted);
    }

    template<typename Handler>
    class FltCommunicationPort
    {
    public:
        FltCommunicationPort() : m_filter(), m_port()
        {
        }

        ~FltCommunicationPort()
        {
            close();
        }

        NTSTATUS create(PFLT_FILTER filter, const UNICODE_STRING& name, LONG maxConnections, bool allowNonAdmins = false)
        {
            ASSERT(!m_port);

            m_filter = filter;

            PSECURITY_DESCRIPTOR securityDescriptor = nullptr;
            NTSTATUS status = ::FltBuildDefaultSecurityDescriptor(&securityDescriptor, FLT_PORT_ALL_ACCESS);

            if (!NT_SUCCESS(status))
            {
                return status;
            }

            SCOPE_EXIT{ ::FltFreeSecurityDescriptor(securityDescriptor); };

            VariableSizeStruct<SYSTEM_MANDATORY_LABEL_ACE, PagedPool> lowIntegrityAce;
            VariableSizeStruct<ACL, PagedPool> sacl;
            if (allowNonAdmins)
            {
                status = RtlSetDaclSecurityDescriptor(securityDescriptor, true, nullptr, false);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }

                const auto lowMandatorySidLength = RtlLengthSid(SeExports->SeLowMandatorySid);
                status = lowIntegrityAce.emplace(FIELD_OFFSET(SYSTEM_MANDATORY_LABEL_ACE, SidStart) + lowMandatorySidLength);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }
                lowIntegrityAce->Header.AceFlags = OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE;
                lowIntegrityAce->Header.AceType = SYSTEM_MANDATORY_LABEL_ACE_TYPE;
                lowIntegrityAce->Header.AceSize = static_cast<USHORT>(FIELD_OFFSET(SYSTEM_MANDATORY_LABEL_ACE, SidStart) + lowMandatorySidLength);
                lowIntegrityAce->Mask = 0;
                status = RtlCopySid(lowMandatorySidLength, &lowIntegrityAce->SidStart, SeExports->SeLowMandatorySid);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }

                const ULONG saclSize = sizeof(ACL) + lowMandatorySidLength + sizeof SYSTEM_MANDATORY_LABEL_ACE;
                status = sacl.emplace(saclSize);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }
                status = RtlCreateAcl(sacl.get(), saclSize, ACL_REVISION);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }

                status = RtlAddAce(sacl.get(), ACL_REVISION, 0, static_cast<PVOID>(lowIntegrityAce.get()), lowIntegrityAce->Header.AceSize);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }

                status = RtlSetSaclSecurityDescriptor(securityDescriptor, true, sacl.get(), false);
                if (!NT_SUCCESS(status))
                {
                    return status;
                }
            }

            ObjectAttributes oa(&name, securityDescriptor);

            return ::FltCreateCommunicationPort(filter, &m_port, &oa, this, connectNotify, disconnectNotify, messageNotify, maxConnections);
        }

        void close()
        {
            if (m_port)
            {
                ::FltCloseCommunicationPort(m_port);
                m_port = nullptr;
            }

            m_filter = nullptr;
        }

    private:
        FltCommunicationPort(const FltCommunicationPort&);
        FltCommunicationPort& operator=(const FltCommunicationPort&);

        static NTSTATUS FLTAPI connectNotify(
            _In_ PFLT_PORT clientPort,
            _In_opt_ PVOID serverPortCookie,
            _In_reads_bytes_opt_(connectionContextLength) PVOID connectionContext,
            _In_ ULONG connectionContextLength,
            _Outptr_result_maybenull_ PVOID* connectionCookie
            )
        {
            ASSERT(serverPortCookie);
            auto self = static_cast<FltCommunicationPort*>(serverPortCookie);
            return Handler::onConnect(self->m_filter, clientPort, connectionContext, connectionContextLength, reinterpret_cast<Handler**>(connectionCookie));
        }

        static VOID FLTAPI disconnectNotify(
            _In_opt_ PVOID connectionCookie
            )
        {
            auto handler = static_cast<Handler*>(connectionCookie);
            handler->onDisconnect();
        }

        static NTSTATUS FLTAPI messageNotify(
            _In_opt_ PVOID connectionCookie,
            _In_reads_bytes_opt_(inputBufferLength) PVOID inputBuffer,
            _In_ ULONG inputBufferLength,
            _Out_writes_bytes_to_opt_(outputBufferLength, *returnOutputBufferLength) PVOID outputBuffer,
            _In_ ULONG outputBufferLength,
            _Out_ PULONG returnOutputBufferLength
            )
        {
            auto handler = static_cast<Handler*>(connectionCookie);

            PMDL inputMdl = nullptr;
            PMDL outputMdl = nullptr;

            NTSTATUS status = [&]()
            {
                //
                // Lock user buffers so __try/__except is required only here and not in the handler->onMessage
                //

                //
                // ATTENTION: Destructors are not called in this function due to __try/__except!
                //

                __try
                {
                    if (inputBufferLength)
                    {
                        inputMdl = IoAllocateMdl(inputBuffer, inputBufferLength, false, false, nullptr);
                        if (!inputMdl)
                        {
                            return STATUS_INSUFFICIENT_RESOURCES;
                        }

                        MmProbeAndLockPages(inputMdl, KernelMode, IoReadAccess);

                        inputBuffer = MmGetSystemAddressForMdlSafe(inputMdl, NormalPagePriority | MdlMappingNoExecute | MdlMappingNoWrite);
                        if (!inputBuffer)
                        {
                            return STATUS_INSUFFICIENT_RESOURCES;
                        }
                    }

                    if (outputBufferLength)
                    {
                        outputMdl = IoAllocateMdl(outputBuffer, outputBufferLength, false, false, nullptr);
                        if (!outputMdl)
                        {
                            return STATUS_INSUFFICIENT_RESOURCES;
                        }

                        MmProbeAndLockPages(outputMdl, KernelMode, IoWriteAccess);

                        outputBuffer = MmGetSystemAddressForMdlSafe(outputMdl, NormalPagePriority | MdlMappingNoExecute);
                        if (!outputBuffer)
                        {
                            return STATUS_INSUFFICIENT_RESOURCES;
                        }
                    }
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                    return STATUS_INVALID_USER_BUFFER;
                }

                return handler->onMessage(inputBuffer, inputBufferLength, outputBuffer, outputBufferLength, returnOutputBufferLength);
            }();

            //
            // Cleanup
            //

            auto freeMdl = [](PMDL& mdl)
            {
                if (mdl)
                {
                    if (FlagOn(mdl->MdlFlags, MDL_PAGES_LOCKED))
                    {
                        MmUnlockPages(mdl);
                    }

                    IoFreeMdl(mdl);
                    mdl = nullptr;
                }
            };

            freeMdl(inputMdl);
            freeMdl(outputMdl);

            return status;
        }

    private:
        PFLT_FILTER m_filter;
        PFLT_PORT   m_port;
    };
} // namespace
