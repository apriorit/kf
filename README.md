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

## Roadmap 
- [ ] Document
- [ ] Add tests
- [ ] Add automated builds
- [ ] Rework `UString`, `USimpleString`, `ASimpleString` in favour of `std::string_view`, `std::wstring_view` as they have no 32k limit
- [ ] Think how to make `GenericTableAvl` safe for concurrent reading (maybe replace it?)
- [x] Get rid of `FltResourceExclusiveLock`/`FltResourceSharedLock`/`EResourceExclusiveLock`/`EResourceSharedLock` and make `FltResource`/`EResource` lockable with `std::shared_lock`/`std::unique_lock`
- [ ] Replace `scoped_buffer` with `vector` (and an appropriate allocator)
- [ ] Update `ConditionVariable` to use `std::unique_lock<Mutex>` where `Mutex` is a template parameter
