#include "sessionhandler.hxx"



namespace Kes
{

namespace Private
{

SessionHandler::~SessionHandler()
{

}

SessionHandler::SessionHandler(const SessionHandlerOptions& options, const std::string& peerAddr)
    : m_options(options)
    , m_peerAddr(peerAddr)
    , m_inBuffer(options.inBufferSize)
{

}

void SessionHandler::close() noexcept
{

}

bool SessionHandler::process(const char* data, size_t size) noexcept
{
    try
    {
        m_inBuffer.push(data, size);


    }
    catch (std::exception& e)
    {
        m_options.log->write(Kes::Log::Level::Error, "SessionHandler: failed to process the packet: %s", e.what());
        return false;
    }

    return true;
}



} // namespace Private {}

} // namespace Kes {}
