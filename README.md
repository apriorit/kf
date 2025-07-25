# kf
Kernel foundation library for Windows.

## Requirements
- Visual Studio 2019 or newer

## Build
To build the project, use CMake with Visual Studio:
```
cmake -S . -B build
```
- For standalone mode, run CMake in the root directory.
- For include mode, add the project as a subdirectory in your own CMake project and link to `kf::kf`.

## Roadmap 
- [ ] Document
- [ ] Add tests
- [ ] Add automated builds
- [ ] Rework `UString`, `USimpleString`, `ASimpleString` in favour of `std::string_view`, `std::wstring_view` as they have no 32k limit
- [ ] Think how to make `GenericTableAvl` safe for concurrent reading (maybe replace it?)
- [x] Get rid of `FltResourceExclusiveLock`/`FltResourceSharedLock`/`EResourceExclusiveLock`/`EResourceSharedLock` and make `FltResource`/`EResource` lockable with `std::shared_lock`/`std::unique_lock`
- [ ] Replace `scoped_buffer` with `vector` (and an appropriate allocator)
- [ ] Update `ConditionVariable` to use `std::unique_lock<Mutex>` where `Mutex` is a template parameter
