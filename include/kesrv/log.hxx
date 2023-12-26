#pragma once

#include <kesrv/kesrv.hxx>


namespace Kes
{

namespace Log
{


enum class Level
{
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
    Off // shoud go last
};


struct ILog
{
    virtual Level level() const noexcept = 0;
    virtual bool writev(Level level, const char* format, va_list args) noexcept = 0;
    virtual bool write(Level level, const char* format, ...) noexcept = 0;

protected:
    virtual ~ILog() {}
};


} // namespace Log {}

} // namespace Kes {}



#define LogDebug(log, ...) \
    log && (log->level() <= Kes::Log::Level::Debug) && log->write(Kes::Log::Level::Debug, __VA_ARGS__)

#define LogInfo(log, ...) \
    log && (log->level() <= Kes::Log::Level::Info) && log->write(Kes::Log::Level::Info, __VA_ARGS__)

#define LogWarning(log, ...) \
    log && (log->level() <= Kes::Log::Level::Warning) && log->write(Kes::Log::Level::Warning, __VA_ARGS__)

#define LogError(log, ...) \
    log && (log->level() <= Kes::Log::Level::Error) && log->write(Kes::Log::Level::Error, __VA_ARGS__)

#define LogFatal(log, ...) \
    log && (log->level() <= Kes::Log::Level::Fatal) && log->write(Kes::Log::Level::Fatal, __VA_ARGS__)




