#pragma once
#define _ITERATOR_DEBUG_LEVEL 0
#include <ntifs.h>
#include <array>
#include <exception>

#pragma warning(push)
#pragma warning(disable: 28159) // Disable "Consider using error logging or driver shutdown instead of KeBugCheckEx"

// These symbols are used by some STL headers and defined here to make linking possible
namespace std
{
    static const ULONG kBugcheckCode = 0xDEAD0001; // Arbitrary bugcheck code for debugging purposes

    [[noreturn]] inline _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xbad_alloc(_In_z_ const char* msg)
    {
        KeBugCheckEx(kBugcheckCode, 1, reinterpret_cast<ULONG_PTR>(msg), 0, 0);
    }
    [[noreturn]] inline _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xinvalid_argument(_In_z_ const char* msg)
    {
        KeBugCheckEx(kBugcheckCode, 2, reinterpret_cast<ULONG_PTR>(msg), 0, 0);
    }

    [[noreturn]] inline _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Xlength_error(_In_z_ const char* msg)
    {
        KeBugCheckEx(kBugcheckCode, 3, reinterpret_cast<ULONG_PTR>(msg), 0, 0);
    }

    [[noreturn]] inline _CRTIMP2_PURE void __CLRCALL_PURE_OR_CDECL _Raise_handler_impl(const stdext::exception& ex)
    {
        KeBugCheckEx(kBugcheckCode, 4, reinterpret_cast<ULONG_PTR>(ex.what()), 0, 0);
    }

    __declspec(selectany) void(__cdecl* _Raise_handler)(const stdext::exception&) = _Raise_handler_impl;
}

#pragma warning(pop)

_ACRTIMP inline void __cdecl _invoke_watson(
    _In_opt_z_ wchar_t const* _Expression,
    _In_opt_z_ wchar_t const* _FunctionName,
    _In_opt_z_ wchar_t const* _FileName,
    _In_       unsigned int _LineNo,
    _In_       uintptr_t _Reserved)
{
    // TODO: assert the expression
}

#include "kmtest/kmtest.h"