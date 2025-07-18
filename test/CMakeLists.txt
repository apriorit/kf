cmake_minimum_required(VERSION 3.5)

project (kftest)

file(GLOB KFTEST_SOURCES "*.h" "*.cpp")
wdk_add_driver(kftest WINVER NTDDI_WIN10 STL ${KFTEST_SOURCES})

target_include_directories(kftest PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_include_directories(kftest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/kmtest/include")