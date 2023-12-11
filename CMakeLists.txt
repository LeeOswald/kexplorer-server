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
set(KES_WINDOWS 0)

set(CMAKE_CXX_STANDARD 20)
set(KES_CXX_FEATURES cxx_std_20)

if(WIN32)
    set(KES_WINDOWS 1)
elseif(UNIX AND NOT APPLE)
    set(KES_LINUX 1)
else()
    message(FATAL_ERROR "Unsupported OS")
endif()

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.in config.h)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party)


# require pthreads
find_package(Threads REQUIRED)
set(PLATFORM_LIBRARIES Threads::Threads)

# require Boost
find_package(Boost REQUIRED COMPONENTS system program_options)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif()

set(BOOST_LIBRARIES Boost::program_options)


set(KES_COMMON kescommon)

if(KES_LINUX)
    set(KES_SERVER kexplorer-server)
endif()

set(KES_CTL kexplorer-ctl)

add_subdirectory(export)
add_subdirectory(kesctl)
if(KES_LINUX)
    add_subdirectory(src)
endif()
