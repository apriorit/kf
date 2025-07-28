cmake_minimum_required(VERSION 3.16)

project(kf-test LANGUAGES CXX)

wdk_add_driver(kf-test WINVER NTDDI_WIN10 STL
    pch.h
    pch.cpp
    HexTest.cpp
    MapTest.cpp
)

target_link_libraries(kf-test kf::kf kmtest::kmtest)

# Activate precompiled headers, unfortunately `target_precompile_headers` doesn't work with `wdk_add_driver`
set_target_properties(kf-test PROPERTIES COMPILE_FLAGS "/Yupch.h")
set_source_files_properties(pch.cpp PROPERTIES COMPILE_FLAGS "/Ycpch.h")
