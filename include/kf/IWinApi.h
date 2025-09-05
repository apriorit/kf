#pragma once

namespace kf
{
    ////////////////////////////////////////////////////
    // Interface for Windows API calls
    class IWinApi
    {
    public:
        virtual NTSTATUS FltBuildDefaultSecurityDescriptor(PSECURITY_DESCRIPTOR* sd, ACCESS_MASK access) = 0;

        virtual VOID FltFreeSecurityDescriptor(PSECURITY_DESCRIPTOR sd) = 0;

        virtual NTSTATUS FltCreateCommunicationPort(
            PFLT_FILTER filter,
            PFLT_PORT* serverPort,
            POBJECT_ATTRIBUTES oa,
            PVOID serverPortCookie,
            PFLT_CONNECT_NOTIFY connectNotify,
            PFLT_DISCONNECT_NOTIFY disconnectNotify,
            PFLT_MESSAGE_NOTIFY messageNotify,
            LONG maxConnections) = 0;

        virtual VOID FltCloseCommunicationPort(PFLT_PORT port) = 0;

        virtual VOID FltCloseClientPort(PFLT_FILTER filter, PFLT_PORT* port) = 0;

        virtual NTSTATUS RtlSetDaclSecurityDescriptor(PSECURITY_DESCRIPTOR sd, BOOLEAN daclPresent, PACL dacl, BOOLEAN daclDefaulted) = 0;

        virtual NTSTATUS RtlSetSaclSecurityDescriptor(PSECURITY_DESCRIPTOR sd, BOOLEAN saclPresent, PACL sacl, BOOLEAN saclDefaulted) = 0;

        virtual ULONG RtlLengthSid(PSID sid) = 0;

        virtual NTSTATUS RtlCopySid(ULONG len, PSID dest, PSID src) = 0;

        virtual NTSTATUS RtlCreateAcl(PACL acl, ULONG size, ULONG rev) = 0;

        virtual NTSTATUS RtlAddAce(PACL acl, ULONG rev, ULONG start, PVOID ace, ULONG aceSize) = 0;

        virtual PMDL IoAllocateMdl(PVOID va, ULONG len, BOOLEAN secondary, BOOLEAN chargeQuota, PIRP irp) = 0;

        virtual VOID IoFreeMdl(PMDL mdl) = 0;

        virtual VOID MmProbeAndLockPages(PMDL mdl, KPROCESSOR_MODE mode, LOCK_OPERATION op) = 0;

        virtual VOID MmUnlockPages(PMDL mdl) = 0;

        virtual PVOID MmGetSystemAddressForMdlSafe(PMDL mdl, ULONG priority) = 0;
    };
}