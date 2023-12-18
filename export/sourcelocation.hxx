#pragma once

#include <export/platform.hxx>

#if defined(__GNUC__) && (__GNUC__ < 11)
    #include <experimental/source_location>

    namespace Kes
    {
        using SourceLocation = std::experimental::source_location;

    } // namespace Kes {}

#else
    #include <source_location>

    namespace Kes
    {
        using SourceLocation = std::source_location;

    } // namespace Kes {}

#endif

