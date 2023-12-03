#include "logger.hxx"

#include <fcntl.h>
#include <sys/file.h>
#include <time.h>

namespace Kes
{

namespace Private
{

Logger::~Logger()
{
    if (m_file >= 0)
        ::close(m_file);
}

Logger::Logger(Kes::Log::Level level, const char* fileName)
    : m_file(::open(fileName, O_WRONLY | O_CREAT | O_TRUNC, S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH))
    , m_level(level)
{
    if (m_file == -1)
        throw std::system_error(std::error_code(errno, std::system_category()), "Failed to create the logfile");

    if (::flock(m_file, LOCK_EX | LOCK_NB) == -1)
    {
        ::close(m_file);

        if (errno == EWOULDBLOCK)
            throw std::runtime_error("Server is already running");

        throw std::system_error(std::error_code(errno, std::system_category()), "Failed to lock the logfile");
    }
}

Kes::Log::Level Logger::level() const noexcept
{
    return m_level;
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

        struct timespec now = {};
        ::clock_gettime(CLOCK_REALTIME, &now);

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

        char timeStr[256];
        ::snprintf(timeStr, _countof(timeStr), "[%02d:%02d:%02d.%03d] ", localNow.tm_hour, localNow.tm_min, localNow.tm_sec, msec);

        std::string message = std::string(timeStr);
        message.append(formatted);
        message.append("\n");

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
    va_list args;
    va_start(args, format);

    auto result = writev(level, format, args);

    va_end(args);

    return result;
}


} // namespace Private {}

} // namespace Kes {}
