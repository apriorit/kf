cmake_minimum_required(VERSION 3.5)

project (kf-benchmark)

file(GLOB KF_BENCHMARK_SOURCES "*.h" "*.cpp")
wdk_add_driver(kf-benchmark WINVER NTDDI_WIN10 STL ${KF_BENCHMARK_SOURCES})

target_include_directories(kf-benchmark PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(kf-benchmark PRIVATE "${kmtest_SOURCE_DIR}/include")