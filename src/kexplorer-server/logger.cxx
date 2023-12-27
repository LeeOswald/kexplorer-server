#include "logger.hxx"

#include <kesrv/exception.hxx>
#include <kesrv/knownprops.hxx>

#include <fcntl.h>
#include <sys/file.h>
#include <time.h>

#include <iostream>

namespace Kes
{

namespace Private
{

Logger::~Logger()
{
}

Logger::Logger(Kes::Log::Level level, const char* fileName)
    : m_file(::open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH))
    , m_level(level)
{
    if (m_file == -1)
        throw Kes::Exception(KES_HERE(), "Failed to create the logfile", Kes::ExceptionProps::PosixErrorCode(errno));

    if (::flock(m_file, LOCK_EX | LOCK_NB) == -1)
    {
        if (errno == EWOULDBLOCK)
            throw Exception(KES_HERE(), "Server is already running");

        throw Kes::Exception(KES_HERE(), "Failed to lock the logfile", Kes::ExceptionProps::PosixErrorCode(errno));
    }
}

Kes::Log::Level Logger::level() const noexcept
{
    return m_level;
}

bool Logger::writev(Kes::Log::Level level, const char* format, va_list args) noexcept
{
    if (level < m_level)
        return true;

    try
    {
        va_list args1;
        va_copy(args1, args);
        auto required = ::vsnprintf(nullptr, 0, format, args1);

        std::string formatted;
        formatted.resize(required);
        ::vsnprintf(formatted.data(), required + 1, format, args);

        va_end(args1);

        struct timespec now = {};
        ::clock_gettime(CLOCK_REALTIME, &now);

        const char* strLevel = "?";
        switch (level)
        {
        case Kes::Log::Level::Debug: strLevel = "D"; break;
        case Kes::Log::Level::Info: strLevel = "I"; break;
        case Kes::Log::Level::Warning: strLevel = "W"; break;
        case Kes::Log::Level::Error: strLevel = "E"; break;
        case Kes::Log::Level::Fatal: strLevel = "!"; break;
        }

        // round nanoseconds to milliseconds
        long msec = 0;
        if (now.tv_nsec >= 999500000)
        {
            now.tv_sec++;
            msec = 0;
        }
        else
        {
            msec = (now.tv_nsec + 500000) / 1000000;
        }

        struct tm localNow = {};
        ::localtime_r(&now.tv_sec, &localNow);

        char prefix[256];
        ::snprintf(prefix, _countof(prefix), "[%02d:%02d:%02d.%03d %s] ", localNow.tm_hour, localNow.tm_min, localNow.tm_sec, msec, strLevel);

        std::string message = std::string(prefix);
        message.append(formatted);
        message.append("\n");

#if KES_DEBUG
        std::cout << message;
#endif

        ::write(m_file, message.data(), message.length());
        ::fsync(m_file);
    }
    catch (...)
    {
        // not much we can do here
    }

    return true;
}

bool Logger::write(Kes::Log::Level level, const char* format, ...) noexcept
{
    if (level < m_level)
        return true;

    va_list args;
    va_start(args, format);

    auto result = writev(level, format, args);

    va_end(args);

    return result;
}


} // namespace Private {}

} // namespace Kes {}
