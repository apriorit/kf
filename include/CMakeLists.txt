cmake_minimum_required(VERSION 3.5)

file(GLOB_RECURSE KF_FILES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/include/*.h"
    "${CMAKE_SOURCE_DIR}/include/*.hpp"
    "${CMAKE_SOURCE_DIR}/include/kf/stl/*"
)

source_group(TREE "${CMAKE_SOURCE_DIR}/include/kf/" FILES ${KF_FILES})

add_custom_target(kf ALL SOURCES ${KF_FILES})
