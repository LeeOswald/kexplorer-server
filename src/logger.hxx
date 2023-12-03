#pragma once

#include <export/log.hxx>


namespace Kes
{

namespace Private
{


class Logger
    : public Kes::Log::ILog
{
public:
    ~Logger();
    explicit Logger(Kes::Log::Level level, const char* fileName);

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    Kes::Log::Level level() const noexcept override;
    bool writev(Kes::Log::Level level, const char* format, va_list args) noexcept override;
    bool write(Kes::Log::Level level, const char* format, ...) noexcept override;


private:
    int m_file = -1;
    Kes::Log::Level m_level = Kes::Log::Level::Info;
};


} // namespace Private {}

} // namespace Kes {}
