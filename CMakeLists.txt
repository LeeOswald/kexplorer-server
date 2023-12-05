cmake_minimum_required(VERSION 3.4)

function(get_version_from_file VER_FILE_NAME)
    file(READ ${VER_FILE_NAME} KES_VERSION_RAW)
    # Remove trailing whitespaces and/or newline
    string(STRIP ${KES_VERSION_RAW} KES_VERSION_)
    set(KES_VERSION ${KES_VERSION_} CACHE STRING
        "Project version determined from version.txt" FORCE
    )
    message(STATUS "Determined project version ${KES_VERSION}")
endfunction()


get_version_from_file("version.txt")

project(KernelExplorer-server VERSION ${KES_VERSION})

set(KES_LINUX 0)

set(CMAKE_CXX_STANDARD 20)
set(KES_CXX_FEATURES cxx_std_20)

if(UNIX AND NOT APPLE)
    set(KES_LINUX 1)
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in config.h)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party)

add_subdirectory(export)
add_subdirectory(kesctl)
add_subdirectory(src)
