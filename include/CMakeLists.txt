cmake_minimum_required(VERSION 3.5)

project(kf)

file(GLOB KF_BOOST_FILES "${CMAKE_CURRENT_LIST_DIR}/kf/boost/*")
file(GLOB KF_EXT_FILES "${CMAKE_CURRENT_LIST_DIR}/kf/ext/*")
file(GLOB KF_STL_FILES "${CMAKE_CURRENT_LIST_DIR}/kf/stl/*")
file(GLOB KF_PATTERNS_FILES "${CMAKE_CURRENT_LIST_DIR}/kf/patterns/*")
file(GLOB KF_ROOT_FILES "${CMAKE_CURRENT_LIST_DIR}/kf/*.h")

source_group("boost" FILES ${KF_BOOST_FILES})
source_group("ext" FILES ${KF_EXT_FILES})
source_group("stl" FILES ${KF_STL_FILES})
source_group("patterns" FILES ${KF_PATTERNS_FILES})
source_group("" FILES ${KF_ROOT_FILES})

wdk_add_library(kf STATIC WINVER NTDDI_WIN10
    ${KF_BOOST_FILES}
    ${KF_EXT_FILES}
    ${KF_STL_FILES}
    ${KF_PATTERNS_FILES}
    ${KF_ROOT_FILES}
)

set_target_properties(kf PROPERTIES LINKER_LANGUAGE CXX)
