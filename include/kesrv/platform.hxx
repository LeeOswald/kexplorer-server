#pragma once

#if defined(_WIN32)
    #define KES_WINDOWS 1
    #define KES_LINUX 0
#elif defined(__linux)
    #define KES_WINDOWS 0
    #define KES_LINUX 1
#else
    #error Unsupported OS
#endif


#if !defined(NDEBUG)
    #define KES_DEBUG 1
#else
    #define KES_DEBUG 0
#endif

#define KES_32 (UINT32_MAX == UINTPTR_MAX)
#define KES_64 (UINT64_MAX == UINTPTR_MAX)

// often-used platform headers


#include <boost/asio.hpp>
#include <boost/stacktrace.hpp>

#if KES_LINUX
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif
#endif

#include <errno.h>
#include <signal.h>

#if KES_LINUX
    #include <sys/types.h>
    #include <unistd.h>
#endif

// often-used std headers
#include <algorithm>
#include <cassert>
#include <climits>
#include <concepts>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <memory>
#include <new>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#ifndef _countof

    template<typename T, size_t N>
    constexpr size_t __countof_impl(T(&arr)[N]) noexcept
    {
        return std::extent<T[N]>::value;
    }

    #define _countof(a) __countof_impl(a)

#endif // _countof

#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif
