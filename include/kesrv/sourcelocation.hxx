#pragma once

#include <kesrv/kesrv.hxx>

#if defined(__GNUC__) && (__GNUC__ < 11)
    #include <experimental/source_location>

    namespace Kes
    {
        using source_location = std::experimental::source_location;

    } // namespace Kes {}

#else
    #include <source_location>

    namespace Kes
    {
        using source_location = std::source_location;

    } // namespace Kes {}

#endif


namespace Kes
{

struct SourceLocation
{
    SourceLocation() noexcept = default;

    SourceLocation(Kes::source_location&& source, boost::stacktrace::stacktrace&& stack)
        : source(std::move(source))
        , stack(std::move(stack))
    {}

    source_location source;
    boost::stacktrace::stacktrace stack;
};

}

#define KES_HERE() Kes::SourceLocation(Kes::source_location::current(), boost::stacktrace::stacktrace())

