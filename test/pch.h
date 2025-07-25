#pragma once
#include <ntifs.h>
#include <array>

// These symbols are used by some STL headers and defined here to make linking possible
inline int _CrtDbgReport(
    _In_       int         _ReportType,
    _In_opt_z_ char const* _FileName,
    _In_       int         _LineNumber,
    _In_opt_z_ char const* _ModuleName,
    _In_opt_z_ char const* _Format,
    ...
)
{
    // TODO: implement proper debug report handling
    return 0;
}

namespace std
{
    [[noreturn]] inline void _Xout_of_range(const char* msg)
    {
        ASSERTMSG(msg, false);
    }
}

#include "kmtest/kmtest.h"