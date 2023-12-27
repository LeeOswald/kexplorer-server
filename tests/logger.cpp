#include "logger.hpp"

#include <iostream>


Kes::Log::ILog* Logger::instance() noexcept
{
    static Logger l;
    return &l;
}

Logger::~Logger()
{
}

Logger::Logger()
{
}

Kes::Log::Level Logger::level() const noexcept
{
    return Kes::Log::Level::Debug;
}

bool Logger::writev(Kes::Log::Level level, const char* format, va_list args) noexcept
{
    try
    {
        va_list args1;
        va_copy(args1, args);
        auto required = ::vsnprintf(nullptr, 0, format, args1);

        std::string formatted;
        formatted.resize(required);
        ::vsnprintf(formatted.data(), required + 1, format, args);

        va_end(args1);

        formatted.append("\n");

        std::cout << formatted;
    }
    catch (...)
    {
        // not much we can do here
    }

    return true;
}

bool Logger::write(Kes::Log::Level level, const char* format, ...) noexcept
{
    va_list args;
    va_start(args, format);

    auto result = writev(level, format, args);

    va_end(args);

    return result;
}
