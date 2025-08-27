#pragma once
#include <ntifs.h>
#include <algorithm>
#include <array>
#include <kmtest/kmtest.h>
#include <fltKernel.h>

// TODO: move this default implementation to kf

///////////////////////////////////////////////////////////
// Implement CRT error reporting and STL checks

extern "C" inline int _CrtDbgReport(
    _In_       int         /*_ReportType*/,
    _In_opt_z_ char const* /*_FileName*/,
    _In_       int         /*_Linenumber*/,
    _In_opt_z_ char const* /*_ModuleName*/,
    _In_opt_z_ char const* /*_Format*/,
    ...)
{
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
    KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
}

namespace std
{
    [[noreturn]] inline void __cdecl _Xinvalid_argument(_In_z_ const char* /*What*/)
    {
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
        KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
    }

    [[noreturn]] inline void __cdecl _Xlength_error(_In_z_ const char* /*What*/)
    {
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
        KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
    }

    [[noreturn]] inline void __cdecl _Xout_of_range(_In_z_ const char* /*What*/)
    {
#pragma warning(suppress: 28159) // Consider using 'error logging or driver shutdown' instead of 'KeBugCheckEx'
        KeBugCheckEx(KERNEL_SECURITY_CHECK_FAILURE, 0, 0, 0, 0);
    }
}
