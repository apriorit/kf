# kf

[![CI](https://github.com/apriorit/kf/actions/workflows/ci.yml/badge.svg)](https://github.com/apriorit/kf/actions/workflows/ci.yml)

Kernel foundation library for Windows.

## Requirements
- Visual Studio 2019 or newer

## How to build tests
Use CMake to generate a Visual Studio solution:

```cmd
cmake -Bbuild .
```

Then open in Visual Studio `<project root>\build\kf.sln` and build it or run CMake:

```cmd
cmake --build build
```

## DoubleLinkedList iteration

`DoubleLinkedList` supports C++20 bidirectional ranges through `begin`/`end`,
`cbegin`/`cend`, and their reverse counterparts. Dereferencing yields a reference
to the linked element; const lists yield const references. The existing
`iterator()` and `descendingIterator()` APIs remain available.

Insertion does not invalidate iterators to existing elements. Removing an
element invalidates iterators to that element; clearing or destroying the list
invalidates iterators to its elements. As with other intrusive containers,
elements must outlive their membership in the list. Algorithms that rearrange
values require suitable element assignment or swapping; iteration itself does
not move or copy elements.

## Roadmap
- [ ] Expand documentation and test coverage.
- [x] Add automated Windows builds (Debug and Release, x64 and Win32).
- [ ] Run kernel tests automatically in CI; the current workflow builds the test driver but does not run it.
- [ ] Rework `UString`, `USimpleString`, and `ASimpleString` in favour of string views where appropriate, avoiding the NT string length limit.
- [ ] Investigate safe concurrent reading for `GenericTableAvl`, including replacement if necessary.
- [x] Make `FltResource` and `EResource` usable with standard lock wrappers.
- [ ] Remove the deprecated `FltResourceExclusiveLock`, `FltResourceSharedLock`, `EResourceExclusiveLock`, and `EResourceSharedLock` wrappers.
- [x] Replace the `scoped_buffer` implementation and usages with `kf::vector` and its allocator; a deprecated forward declaration remains.
- [ ] Update `ConditionVariable` to use `std::unique_lock<Mutex>` where `Mutex` is a template parameter.
- [x] Support C++20 bidirectional ranges and algorithms in `DoubleLinkedList`.

## About Apriorit

This repository is a public technical example from Apriorit related to system programming and advanced software engineering.

Apriorit is a cybersecurity-focused software engineering company that helps technology companies build secure, reliable, and high-performance software — from kernel-level components to cloud-based platforms.

Learn more: https://www.apriorit.com/our-expertise/kernel-and-driver-development

Website: https://www.apriorit.com
