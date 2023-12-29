#include "requestprocessor.hxx"
#include "sessionhandler.hxx"

#include <kesrv/exception.hxx>


namespace Kes
{

namespace Private
{

SessionHandler::~SessionHandler()
{
    m_options.requestProcessor->endSession(m_id);
}

SessionHandler::SessionHandler(const SessionHandlerOptions& options, const std::string& peerAddr, uint32_t id)
    : m_options(options)
    , m_peerAddr(peerAddr)
    , m_id(id)
    , m_buffer(options.bufferSize, options.bufferLimit)
{
    m_options.requestProcessor->startSession(m_id);
}

void SessionHandler::close() noexcept
{

}

std::pair<CallbackResult, std::string> SessionHandler::process(const char* data, size_t size) noexcept
{
    std::string response;

    try
    {
        auto posPrev = m_buffer.used();

        if (!m_buffer.push(data, size))
            throw Exception(KES_HERE(), "Packet size exceeds limit");

        auto posCur = m_buffer.used();

        if (posCur == posPrev)
        {
            // nothing to process
            return std::make_pair(CallbackResult::Continue, response);
        }

        auto cur = m_buffer.data() + posPrev;
        auto end = m_buffer.data() + posCur;
        while (cur < end)
        {
            if (*cur == '{')
            {
                ++m_jsonDepth;
                if (m_jsonDepth > m_jsonDepthMax)
                    m_jsonDepthMax = m_jsonDepth;
            }
            else if (*cur == '}')
            {
                if (m_jsonDepth == 0)
                    throw Exception(KES_HERE(), "Invalid JSON");

                --m_jsonDepth;
            }

            ++cur;
        }

        if (m_jsonDepth == 0)
        {
            // JSON complete
            if (m_jsonDepthMax == 0)
            {
                // nothing to process
                return std::make_pair(CallbackResult::Continue, response);
            }

            m_buffer.push("", 1); // append '\0'

            response = m_options.requestProcessor->process(m_id, m_buffer.data(), posCur);

            m_buffer.pop(posCur + 1); // also pop '\0'
            m_jsonDepthMax = 0;
        }
    }
    catch (std::exception& e)
    {
        m_options.log->write(Kes::Log::Level::Error, "SessionHandler: failed to process the request: %s", e.what());
        m_buffer.reset();
        m_jsonDepth = 0;
        m_jsonDepthMax = 0;
        return std::make_pair(CallbackResult::Abort, response); // server should reset the connection in this case
    }

    return std::make_pair(CallbackResult::Continue, response);
}


} // namespace Private {}

} // namespace Kes {}
