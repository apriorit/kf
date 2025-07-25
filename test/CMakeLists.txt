cmake_minimum_required(VERSION 3.11)

project ("kf-test")

file(GLOB KFTEST_SOURCES "*.h" "*.cpp")
wdk_add_driver("kf-test" WINVER NTDDI_WIN10 STL ${KFTEST_SOURCES})

target_include_directories(kf-test PRIVATE "${CMAKE_SOURCE_DIR}/include")

include(FetchContent)

FetchContent_Declare(
    kmtest
    GIT_REPOSITORY https://github.com/SergiusTheBest/kmtest.git
)

FetchContent_Populate(kmtest)

target_include_directories(kf-test PRIVATE "${kmtest_SOURCE_DIR}/include")
