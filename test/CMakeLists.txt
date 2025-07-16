cmake_minimum_required(VERSION 3.5)

project (kftest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/FindWDK/cmake")
find_package(WDK REQUIRED)

file(GLOB KFTEST_SOURCES "*.h" "*.cpp")
wdk_add_driver(kftest WINVER NTDDI_WIN10 ${KFTEST_SOURCES})

target_include_directories(kftest PRIVATE "${CMAKE_CURRENT_LIST_DIR}/kmtest/include")
