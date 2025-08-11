cmake_minimum_required(VERSION 3.16)

project(kf-test LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set warning level to /W4 for MSVC
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")

# TODO: fix warnings in code and remove this define
add_compile_definitions(_CRT_SECURE_NO_WARNINGS=1)

# Fetch dependencies
include(FetchContent)

# TODO: add stable reference in future
FetchContent_Declare(
    findwdk
    GIT_REPOSITORY https://github.com/SergiusTheBest/FindWDK
    GIT_TAG stl
)

# TODO: add stable reference in future
FetchContent_Declare(
    kmtest
    GIT_REPOSITORY https://github.com/SergiusTheBest/kmtest.git
)

FetchContent_MakeAvailable(findwdk kmtest)

# Add CMake package for WDK projects
list(APPEND CMAKE_MODULE_PATH "${findwdk_SOURCE_DIR}/cmake")
find_package(WDK REQUIRED)

wdk_add_driver(kf-test WINVER NTDDI_WIN10 STL
    pch.h
    pch.cpp
    AdjacentView.cpp
    Bitmap.cpp
    BitmapRangeIterator.cpp
    HexTest.cpp
    MapTest.cpp
    Vector.cpp
    EncodingDetectorTest.cpp
)

target_link_libraries(kf-test kf::kf kmtest::kmtest)

# Activate precompiled headers, unfortunately `target_precompile_headers` doesn't work with `wdk_add_driver`
set_target_properties(kf-test PROPERTIES COMPILE_FLAGS "/Yupch.h")
set_source_files_properties(pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")

# TODO: move it to FindWDK
target_compile_definitions(kf-test PRIVATE _ITERATOR_DEBUG_LEVEL=0)
