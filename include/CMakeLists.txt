cmake_minimum_required(VERSION 3.5)

project(kf)

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/kf>
)

set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

# Add a pseudo-project to make kf headers visible in IDE
file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/kf/*)

# Organize files into filters matching their relative paths
foreach(header ${${PROJECT_NAME}_HEADERS})
    file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}" "${header}")
    get_filename_component(rel_dir "${rel_path}" DIRECTORY)
    if(rel_dir)
        string(REPLACE "/" "\\" filter "${rel_dir}")
        source_group("${filter}" FILES "${header}")
    endif()
endforeach()

add_custom_target(${PROJECT_NAME}-headers SOURCES ${${PROJECT_NAME}_HEADERS})