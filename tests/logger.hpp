#pragma once


#include <kesrv/log.hxx>


class Logger final
    : public Kes::Log::ILog
{
public:
    static Kes::Log::ILog* instance() noexcept;

    ~Logger();
    Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    Kes::Log::Level level() const noexcept override;
    bool writev(Kes::Log::Level level, const char* format, va_list args) noexcept override;
    bool write(Kes::Log::Level level, const char* format, ...) noexcept override;
};
