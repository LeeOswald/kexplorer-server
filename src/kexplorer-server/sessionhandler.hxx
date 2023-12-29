#pragma once

#include <kesrv/log.hxx>
#include <kesrv/util/continuousbuffer.hxx>


namespace Kes
{

namespace Private
{


struct SessionHandlerOptions
{
    size_t bufferSize;
    size_t bufferLimit;
    IRequestProcessor* requestProcessor;
    Kes::Log::ILog* log;

    explicit SessionHandlerOptions(size_t bufferSize, size_t bufferLimit, IRequestProcessor* requestProcessor, Kes::Log::ILog* log)
        : bufferSize(bufferSize)
        , bufferLimit(bufferLimit)
        , requestProcessor(requestProcessor)
        , log(log)
    {}
};


class SessionHandler final
{
public:
    ~SessionHandler();
    explicit SessionHandler(const SessionHandlerOptions& options, const std::string& peerAddr, uint32_t id);

    SessionHandler(const SessionHandler&) = delete;
    SessionHandler& operator=(const SessionHandler&) = delete;

    SessionHandler(SessionHandler&&) = delete;
    SessionHandler& operator=(SessionHandler&&) = delete;

    void close() noexcept;
    std::pair<CallbackResult, std::string> process(const char* data, size_t size) noexcept;
    const std::string& peer() const noexcept { return m_peerAddr; }

private:
    SessionHandlerOptions m_options;
    std::string m_peerAddr;
    uint32_t m_id;
    Kes::Util::ContinuousBuffer m_buffer;
    size_t m_jsonDepth = 0;
    size_t m_jsonDepthMax = 0;
};



} // namespace Private {}

} // namespace Kes {}
