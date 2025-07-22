cmake_minimum_required(VERSION 3.5)

project (kfbenchmark)

file(GLOB KF_BENCHMARK_SOURCES "*.h" "*.cpp")
wdk_add_driver(kfbenchmark WINVER NTDDI_WIN10 STL ${KF_BENCHMARK_SOURCES})

target_include_directories(kfbenchmark PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(kfbenchmark PRIVATE "${CMAKE_SOURCE_DIR}/test/kmtest/include")