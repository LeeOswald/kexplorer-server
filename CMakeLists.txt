cmake_minimum_required(VERSION 3.20)

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

project(kexplorer-server VERSION ${KES_VERSION})

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "CMAKE_BUILD_TYPE isn't defined. Debug value will be used by default.")
    set(CMAKE_BUILD_TYPE Debug)
else()
    if(${CMAKE_BUILD_TYPE} MATCHES "^[Rr][Ee][Ll][Ee][Aa][Ss][Ee]$")
        set(CMAKE_BUILD_TYPE Release)
    elseif(${CMAKE_BUILD_TYPE} MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
        set(CMAKE_BUILD_TYPE Debug)
    else()
        message(WARNING "CMAKE_BUILD_TYPE isn't Release or Debug. Debug value will be used by default.")
        set(CMAKE_BUILD_TYPE Debug)
    endif()
endif()

if(MSVC)
else()
    add_link_options(-rdynamic) # enable stack capture
endif()

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

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kexplorer-version.h.in kexplorer-version.h)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
include_directories(${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/3rd_party ${CMAKE_CURRENT_SOURCE_DIR}/include)


# require pthreads
find_package(Threads REQUIRED)
set(PLATFORM_LIBRARIES Threads::Threads)

# require Boost
find_package(Boost REQUIRED COMPONENTS system program_options)

include_directories(${Boost_INCLUDE_DIRS})
link_directories(${Boost_LIBRARY_DIRS})

set(BOOST_LIBRARIES Boost::program_options)

if(WIN32)
    # disable autolinking in boost
    add_definitions(-DBOOST_ALL_NO_LIB)
    # force all boost libraries to dynamic link
    add_definitions(-DBOOST_ALL_DYN_LINK)
endif()

# force shared CRT
if(MSVC)
    cmake_policy(SET CMP0091 NEW)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Windows-specific settings
if(WIN32)
    add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# component names
set(KES_SRVLIB kesrv)

if(KES_LINUX)
    set(KES_SERVER kexplorer-server)
endif()

set(KES_CTL kexplorer-ctl)

# components
add_subdirectory(3rd_party/googletest)
add_subdirectory(src/kesrv)
add_subdirectory(src/kesctl)
if(KES_LINUX)
    add_subdirectory(src/kexplorer-server)
endif()
add_subdirectory(tests)
