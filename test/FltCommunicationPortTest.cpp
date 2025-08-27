#include "pch.h"
#include "WinApiMock.h"
#include <kf/FltCommunicationPort.h>

namespace
{
    class SampleHandler
    {
    public:
        static NTSTATUS onConnect(
            PFLT_FILTER filter,
            PFLT_PORT clientPort,
            _In_reads_bytes_opt_(connectionContextLength) PVOID connectionContext,
            ULONG connectionContextLength,
            _Outptr_ SampleHandler** handler,
            _In_ kf::IWinApi& api)
        {
            UNREFERENCED_PARAMETER(connectionContext);
            UNREFERENCED_PARAMETER(connectionContextLength);
            *handler = new(PagedPool) SampleHandler(filter, clientPort, api);

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
        SampleHandler(PFLT_FILTER filter, PFLT_PORT clientPort, kf::IWinApi& api) : m_filter(filter), m_clientPort(clientPort), m_api(api)
        {
        }

        ~SampleHandler()
        {
            m_api.FltCloseClientPort(m_filter, &m_clientPort);
        }

    public:
        kf::IWinApi& m_api;

    private:
        PFLT_FILTER m_filter;
        PFLT_PORT m_clientPort;
    };
}

SCENARIO("FltCommunicationPort creation and cleanup with mocks")
{
    GIVEN("WinApiMock and FltCommunicationPort instance")
    {
        WinApiMock apiMock;
        kf::FltCommunicationPort<SampleHandler> port(apiMock);

        UNICODE_STRING name{};
        PFLT_FILTER filter = reinterpret_cast<PFLT_FILTER>(1);

        WHEN("create() is called with valid parameters")
        {
            NTSTATUS status = port.create(filter, name, 1);

            THEN("Port is created successful and cleanup frees descriptor")
            {
                REQUIRE_NT_SUCCESS(status);
            }
        }

        WHEN("create() fails due to FltBuildDefaultSecurityDescriptor")
        {
            apiMock.m_funcs.FltBuildDefaultSecurityDescriptor = [](PSECURITY_DESCRIPTOR*, ACCESS_MASK) {
                return STATUS_INSUFFICIENT_RESOURCES;
                };

            NTSTATUS status = port.create(filter, name, 1);

            THEN("create() returns status of the failure")
            {
                REQUIRE(status == STATUS_INSUFFICIENT_RESOURCES);
            }
        }

        WHEN("create() fails due to FltCreateCommunicationPort")
        {
            apiMock.m_funcs.FltCreateCommunicationPort =
                [](auto, PFLT_PORT* serverPort, auto, auto, auto, auto, auto, auto) {
                *serverPort = nullptr;
                return STATUS_INSUFF_SERVER_RESOURCES;
                };

            static bool freeCalled = false;
            apiMock.m_funcs.FltFreeSecurityDescriptor = [](PSECURITY_DESCRIPTOR) { freeCalled = true; };

            NTSTATUS status = port.create(filter, name, 1);

            THEN("create() returns status of the failure and frees the descriptor")
            {
                REQUIRE(status == STATUS_INSUFF_SERVER_RESOURCES);
                REQUIRE(freeCalled);
            }
        }

        WHEN("create() called with allowNonAdmins triggers ACL/SACL setup and fails")
        {
            apiMock.m_funcs.RtlSetDaclSecurityDescriptor = [](auto, auto, auto, auto) { return STATUS_ACCESS_DENIED; };

            NTSTATUS status = port.create(filter, name, 1, true);

            THEN("create() returns status error")
            {
                REQUIRE(status == STATUS_ACCESS_DENIED);
            }
        }

        WHEN("Closing twice does not crash")
        {
            port.create(filter, name, 1);
            port.close();
            port.close();

            THEN("no crash occurs")
            {
            }
        }

        WHEN("close() is called without prior create")
        {
            port.close();

            THEN("no crash occurs")
            {
            }
        }

        WHEN("Creating port after it was closed")
        {
            REQUIRE_NT_SUCCESS(port.create(filter, name, 1));
            port.close();

            NTSTATUS status2 = port.create(filter, name, 1);
            THEN("Second create() succeeds after close()")
            {
                REQUIRE_NT_SUCCESS(status2);
            }
        }
    }

    GIVEN("Successfully created FltCommunicationPort")
    {
        WinApiMock apiMock;
        static bool freeCalled = false;
        apiMock.m_funcs.FltCloseCommunicationPort = [](PFLT_PORT) { freeCalled = true; };

        {
            kf::FltCommunicationPort<SampleHandler> port(apiMock);
            UNICODE_STRING name{};
            PFLT_FILTER filter = reinterpret_cast<PFLT_FILTER>(1);

            REQUIRE_NT_SUCCESS(port.create(filter, name, 1));
        }

        WHEN("FltCreateCommunicationPort goes out of scope")
        {
            THEN("FltCreateCommunicationPort closes the port")
            {
                REQUIRE(freeCalled);
            }
        }
    }
}
