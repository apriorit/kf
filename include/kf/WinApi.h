#pragma once
#include "IWinApi.h"

namespace kf
{    
    //////////////////////////////////////////////////////////////////////////
    // Wrapper for Windows API calls to allow mocking in unit tests
    class WinApi : public IWinApi
    {
    public:
        NTSTATUS FltBuildDefaultSecurityDescriptor(PSECURITY_DESCRIPTOR* sd, ACCESS_MASK access)
        {
            return ::FltBuildDefaultSecurityDescriptor(sd, access);
        }

        VOID FltFreeSecurityDescriptor(PSECURITY_DESCRIPTOR sd)
        {
            ::FltFreeSecurityDescriptor(sd);
        }
        NTSTATUS FltCreateCommunicationPort(
            PFLT_FILTER filter,
            PFLT_PORT* serverPort,
            POBJECT_ATTRIBUTES oa,
            PVOID serverPortCookie,
            PFLT_CONNECT_NOTIFY connectNotify,
            PFLT_DISCONNECT_NOTIFY disconnectNotify,
            PFLT_MESSAGE_NOTIFY messageNotify,
            LONG maxConnections)
        {
            return ::FltCreateCommunicationPort(filter, serverPort, oa, serverPortCookie, connectNotify, disconnectNotify, messageNotify, maxConnections);
        }

        VOID FltCloseCommunicationPort(PFLT_PORT port)
        {
            ::FltCloseCommunicationPort(port);
        }

        VOID FltCloseClientPort(PFLT_FILTER filter, PFLT_PORT* port)
        {
            ::FltCloseClientPort(filter, port);
        }

        NTSTATUS RtlSetDaclSecurityDescriptor(PSECURITY_DESCRIPTOR sd, BOOLEAN daclPresent, PACL dacl, BOOLEAN daclDefaulted)
        {
            return ::RtlSetDaclSecurityDescriptor(sd, daclPresent, dacl, daclDefaulted);
        }

        NTSTATUS RtlSetSaclSecurityDescriptor(PSECURITY_DESCRIPTOR sd, BOOLEAN saclPresent, PACL sacl, BOOLEAN saclDefaulted)
        {
            return ::RtlSetSaclSecurityDescriptor(sd, saclPresent, sacl, saclDefaulted);
        }

        ULONG RtlLengthSid(PSID sid)
        {
            return ::RtlLengthSid(sid);
        }

        NTSTATUS RtlCopySid(ULONG len, PSID dest, PSID src)
        {
            return ::RtlCopySid(len, dest, src);
        }

        NTSTATUS RtlCreateAcl(PACL acl, ULONG size, ULONG rev)
        {
            return ::RtlCreateAcl(acl, size, rev);
        }

        NTSTATUS RtlAddAce(PACL acl, ULONG rev, ULONG start, PVOID ace, ULONG aceSize)
        {
            return ::RtlAddAce(acl, rev, start, ace, aceSize);
        }

        PMDL IoAllocateMdl(PVOID va, ULONG len, BOOLEAN secondary, BOOLEAN chargeQuota, PIRP irp)
        {
            return ::IoAllocateMdl(va, len, secondary, chargeQuota, irp);
        }

        VOID IoFreeMdl(PMDL mdl)
        {
            ::IoFreeMdl(mdl);
        }

        VOID MmProbeAndLockPages(PMDL mdl, KPROCESSOR_MODE mode, LOCK_OPERATION op)
        {
            ::MmProbeAndLockPages(mdl, mode, op);
        }

        VOID MmUnlockPages(PMDL mdl)
        {
            ::MmUnlockPages(mdl);
        }

        PVOID MmGetSystemAddressForMdlSafe(PMDL mdl, ULONG priority)
        {
            return ::MmGetSystemAddressForMdlSafe(mdl, priority);
        }
    };
}
