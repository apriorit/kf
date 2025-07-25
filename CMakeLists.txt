cmake_minimum_required(VERSION 3.11)

project(kf)

# check if building as a stand-alone project
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

    # Set C++ standard
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    # include subproject for kf headers
    add_subdirectory("include")

    # Fetch dependencies
    include(FetchContent)
    FetchContent_Declare(
        findwdk
        GIT_REPOSITORY https://github.com/SergiusTheBest/FindWDK
        GIT_TAG stl
    )
    FetchContent_Declare(
        kmtest
        GIT_REPOSITORY https://github.com/SergiusTheBest/kmtest.git
    )

    FetchContent_Populate(findwdk)
    FetchContent_Populate(kmtest)

    # Add CMake package for WDK projects
    list(APPEND CMAKE_MODULE_PATH "${findwdk_SOURCE_DIR}/cmake")
    find_package(WDK REQUIRED)

    # Add other solution projects
    add_subdirectory("test")

    # Add all .md files from the root directory to the solution
    file(GLOB ROOT_MD_FILES "${CMAKE_CURRENT_SOURCE_DIR}/*.md")
    add_custom_target("docs" SOURCES ${ROOT_MD_FILES})

endif()