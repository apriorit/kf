#pragma once
#include <kf/IWinApi.h>

struct WinApiMockFuncs
{
    NTSTATUS(*FltCreateCommunicationPort)(
        PFLT_FILTER, PFLT_PORT* serverPort, POBJECT_ATTRIBUTES, PVOID,
        PFLT_CONNECT_NOTIFY, PFLT_DISCONNECT_NOTIFY, PFLT_MESSAGE_NOTIFY, LONG) =
        [](PFLT_FILTER, PFLT_PORT* serverPort, POBJECT_ATTRIBUTES, PVOID,
            PFLT_CONNECT_NOTIFY, PFLT_DISCONNECT_NOTIFY, PFLT_MESSAGE_NOTIFY, LONG)
        {
            *serverPort = reinterpret_cast<PFLT_PORT>(1);
            return STATUS_SUCCESS;
        };

    NTSTATUS(*FltBuildDefaultSecurityDescriptor)(PSECURITY_DESCRIPTOR*, ACCESS_MASK) =
        [](PSECURITY_DESCRIPTOR*, ACCESS_MASK)
        {
            return STATUS_SUCCESS;
        };

    VOID(*FltFreeSecurityDescriptor)(PSECURITY_DESCRIPTOR) = [](PSECURITY_DESCRIPTOR) {};

    VOID(*FltCloseCommunicationPort)(PFLT_PORT) = [](PFLT_PORT) {};

    VOID(*FltCloseClientPort)(PFLT_FILTER, PFLT_PORT*) = [](PFLT_FILTER, PFLT_PORT*) {};

    NTSTATUS(*RtlSetDaclSecurityDescriptor)(PSECURITY_DESCRIPTOR, BOOLEAN, PACL, BOOLEAN) =
        [](PSECURITY_DESCRIPTOR, BOOLEAN, PACL, BOOLEAN) { return STATUS_SUCCESS; };

    NTSTATUS(*RtlSetSaclSecurityDescriptor)(PSECURITY_DESCRIPTOR, BOOLEAN, PACL, BOOLEAN) =
        [](PSECURITY_DESCRIPTOR, BOOLEAN, PACL, BOOLEAN) { return STATUS_SUCCESS; };

    ULONG(*RtlLengthSid)(PSID) = [](PSID) { return static_cast<ULONG>(0); };

    NTSTATUS(*RtlCopySid)(ULONG, PSID, PSID) =
        [](ULONG, PSID, PSID) { return STATUS_SUCCESS; };

    NTSTATUS(*RtlCreateAcl)(PACL, ULONG, ULONG) =
        [](PACL, ULONG, ULONG) { return STATUS_SUCCESS; };

    NTSTATUS(*RtlAddAce)(PACL, ULONG, ULONG, PVOID, ULONG) =
        [](PACL, ULONG, ULONG, PVOID, ULONG) { return STATUS_SUCCESS; };

    PMDL(*IoAllocateMdl)(PVOID, ULONG, BOOLEAN, BOOLEAN, PIRP) =
        [](PVOID, ULONG, BOOLEAN, BOOLEAN, PIRP) { return reinterpret_cast<PMDL>(1); };

    VOID(*IoFreeMdl)(PMDL) =
        [](PMDL) {};

    VOID(*MmProbeAndLockPages)(PMDL, KPROCESSOR_MODE, LOCK_OPERATION) =
        [](PMDL, KPROCESSOR_MODE, LOCK_OPERATION) {};

    VOID(*MmUnlockPages)(PMDL) =
        [](PMDL) {};

    PVOID(*MmGetSystemAddressForMdlSafe)(PMDL, ULONG) =
        [](PMDL, ULONG) { return reinterpret_cast<PVOID>(1); };
};


class WinApiMock : public kf::IWinApi
{
public:
    NTSTATUS FltBuildDefaultSecurityDescriptor(PSECURITY_DESCRIPTOR* sd, ACCESS_MASK access)
    {
        return m_funcs.FltBuildDefaultSecurityDescriptor(sd, access);
    }

    VOID FltFreeSecurityDescriptor(PSECURITY_DESCRIPTOR sd)
    {
        m_funcs.FltFreeSecurityDescriptor(sd);
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
        return m_funcs.FltCreateCommunicationPort(filter, serverPort, oa, serverPortCookie, connectNotify, disconnectNotify, messageNotify, maxConnections);
    }

    VOID FltCloseCommunicationPort(PFLT_PORT port)
    {
        m_funcs.FltCloseCommunicationPort(port);
    }

    VOID FltCloseClientPort(PFLT_FILTER filter, PFLT_PORT* port)
    {
        m_funcs.FltCloseClientPort(filter, port);
    }

    NTSTATUS RtlSetDaclSecurityDescriptor(PSECURITY_DESCRIPTOR sd, BOOLEAN daclPresent, PACL dacl, BOOLEAN daclDefaulted)
    {
        return m_funcs.RtlSetDaclSecurityDescriptor(sd, daclPresent, dacl, daclDefaulted);
    }

    NTSTATUS RtlSetSaclSecurityDescriptor(PSECURITY_DESCRIPTOR sd, BOOLEAN saclPresent, PACL sacl, BOOLEAN saclDefaulted)
    {
        return m_funcs.RtlSetSaclSecurityDescriptor(sd, saclPresent, sacl, saclDefaulted);
    }

    ULONG RtlLengthSid(PSID sid)
    {
        return m_funcs.RtlLengthSid(sid);
    }

    NTSTATUS RtlCopySid(ULONG len, PSID dest, PSID src)
    {
        return m_funcs.RtlCopySid(len, dest, src);
    }

    NTSTATUS RtlCreateAcl(PACL acl, ULONG size, ULONG rev)
    {
        return m_funcs.RtlCreateAcl(acl, size, rev);
    }

    NTSTATUS RtlAddAce(PACL acl, ULONG rev, ULONG start, PVOID ace, ULONG aceSize)
    {
        return m_funcs.RtlAddAce(acl, rev, start, ace, aceSize);
    }

    PMDL IoAllocateMdl(PVOID va, ULONG len, BOOLEAN secondary, BOOLEAN chargeQuota, PIRP irp)
    {
        return m_funcs.IoAllocateMdl(va, len, secondary, chargeQuota, irp);
    }

    VOID IoFreeMdl(PMDL mdl)
    {
        m_funcs.IoFreeMdl(mdl);
    }

    VOID MmProbeAndLockPages(PMDL mdl, KPROCESSOR_MODE mode, LOCK_OPERATION op)
    {
        m_funcs.MmProbeAndLockPages(mdl, mode, op);
    }

    VOID MmUnlockPages(PMDL mdl)
    {
        m_funcs.MmUnlockPages(mdl);
    }

    PVOID MmGetSystemAddressForMdlSafe(PMDL mdl, ULONG priority)
    {
        return m_funcs.MmGetSystemAddressForMdlSafe(mdl, priority);
    }

public:
    WinApiMockFuncs m_funcs;
};
