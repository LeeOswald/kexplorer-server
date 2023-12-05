#include "sessionhandler.hxx"

#include <export/json.hxx>


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
    , m_buffer(options.bufferSize, options.bufferLimit)
{

}

void SessionHandler::close() noexcept
{

}

std::pair<bool, std::string> SessionHandler::process(const char* data, size_t size) noexcept
{
    std::string response;

    try
    {
        auto posPrev = m_buffer.used();

        if (!m_buffer.push(data, size))
            throw std::runtime_error("Packet size exceeds limit");

        auto posCur = m_buffer.used();

        if (posCur == posPrev)
        {
            // nothing to process
            return std::make_pair(true, response);
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
                    throw std::runtime_error("Invalid JSON");

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
                return std::make_pair(true, response);
            }

            m_buffer.push("", 1); // append '\0'

            response = processJson(m_buffer.data(), posCur);

            m_buffer.pop(posCur + 1); // also pop '\0'
            m_jsonDepthMax = 0;
        }
    }
    catch (std::exception& e)
    {
        m_options.log->write(Kes::Log::Level::Error, "SessionHandler: failed to process the packet: %s", e.what());
        m_buffer.reset();
        m_jsonDepth = 0;
        m_jsonDepthMax = 0;
        return std::make_pair(false, response); // server should reset the connection in this case
    }

    return std::make_pair(true, response);
}

std::string SessionHandler::processJson(const char* json, size_t length)
{
    LogDebug(m_options.log, "%s", json);

    Kes::Json::Document doc;
    doc.Parse(json, length);

    if (!doc.IsObject())
        throw std::runtime_error("Not a JSON object");

    auto commandKey = doc.FindMember("command");
    //assert(hello != document.MemberEnd());
    //    assert(hello->value.IsString());
    //    assert(strcmp("world", hello->value.GetString()) == 0);

    return std::string();
}

} // namespace Private {}

} // namespace Kes {}
