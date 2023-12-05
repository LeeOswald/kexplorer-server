#pragma once

#include <export/condition.hxx>
#include <export/log.hxx>
#include <export/util/continuousbuffer.hxx>


namespace Kes
{

namespace Private
{


struct SessionHandlerOptions
{
    size_t bufferSize;
    size_t bufferLimit;
    Kes::Condition* exit;
    Kes::Log::ILog* log;

    explicit SessionHandlerOptions(size_t bufferSize, size_t bufferLimit, Kes::Condition* exit, Kes::Log::ILog* log)
        : bufferSize(bufferSize)
        , bufferLimit(bufferLimit)
        , exit(exit)
        , log(log)
    {}
};


class SessionHandler final
{
public:
    ~SessionHandler();
    explicit SessionHandler(const SessionHandlerOptions& options, const std::string& peerAddr);

    SessionHandler(const SessionHandler&) = delete;
    SessionHandler& operator=(const SessionHandler&) = delete;

    SessionHandler(SessionHandler&&) = delete;
    SessionHandler& operator=(SessionHandler&&) = delete;

    void close() noexcept;
    std::pair<bool, std::string> process(const char* data, size_t size) noexcept;
    const std::string& peer() const noexcept { return m_peerAddr; }

private:
    std::string processJson(const char* json, size_t length);

    SessionHandlerOptions m_options;
    std::string m_peerAddr;
    Kes::Util::ContinuousBuffer m_buffer;
    size_t m_jsonDepth = 0;
    size_t m_jsonDepthMax = 0;
};



} // namespace Private {}

} // namespace Kes {}
