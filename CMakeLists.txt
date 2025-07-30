cmake_minimum_required(VERSION 3.16)

project(kf LANGUAGES CXX)

# Check if building as a stand-alone project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

add_library(${PROJECT_NAME} INTERFACE)
add_library(${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}
    INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

if(${IS_TOPLEVEL_PROJECT})
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Add a pseudo-project to make kf headers visible in IDE
    file(GLOB_RECURSE ${PROJECT_NAME}_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/kf/*")

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
    set_target_properties(${PROJECT_NAME} PROPERTIES LINKER_LANGUAGE CXX)

    # Add all .md files from the root directory to the solution
    file(GLOB ROOT_MD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.md")
    add_custom_target("docs" SOURCES ${ROOT_MD_FILES})

    # Add tests (can be disabled with -DKF_BUILD_TESTS=OFF)
    option(KF_BUILD_TESTS "Build kf tests (requires WDK)" ON)
    if(KF_BUILD_TESTS)
        add_subdirectory("test")
    endif()
endif()

# TODO: add install target
