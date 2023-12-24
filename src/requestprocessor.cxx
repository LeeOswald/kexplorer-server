#include "requestprocessor.hxx"

#include <export/exception.hxx>
#include <export/util/request.hxx>


namespace Kes
{

namespace Private
{

std::string RequestProcessor::process(uint32_t sessionId, const char* request, size_t length)
{
    try
    {
        LogDebug(m_log, "\n-> %s\n", request);

        Kes::Json::Document docRequest;
        docRequest.Parse(request, length);

        if (!docRequest.IsObject())
        {
            m_log->write(Log::Level::Error, "RequestProcessor: request is not a JSON object");
            return Util::Response::fail("Not a JSON object");
        }

        auto requestKey = docRequest.FindMember("request");
        if (requestKey != docRequest.MemberEnd())
        {
            auto key = requestKey->value.GetString();
            if (!key)
            {
                m_log->write(Log::Level::Error, "RequestProcessor: invalid request");
                return Util::Response::fail("Invalid request");
            }

            Kes::Json::Document docResponse;
            docResponse.SetObject();

            bool handlerFound = false;
            {
                std::lock_guard l(m_mutex);

                auto range = m_handlers.equal_range(key);
                for (auto it = range.first; it != range.second; ++it)
                {
                    auto handler = it->second;
                    handlerFound = handler->process(sessionId, key, docRequest, docResponse);
                }
            }

            if (!handlerFound)
            {
                m_log->write(Log::Level::Error, "RequestProcessor: unsupported request");
                return Util::Response::fail("Unsupported request");
            }

            Json::StringBuffer sb;
            Json::Writer<Json::StringBuffer> writer(sb);
            docResponse.Accept(writer);
            auto out = sb.GetString();

            LogDebug(m_log, "\n<- %s\n", out);

            return std::string(out);
        }
    }
    catch (std::exception& e)
    {
        m_log->write(Log::Level::Error, "RequestProcessor: %s", e.what());
        return Util::Response::fail(e.what());
    }

    m_log->write(Log::Level::Error, "RequestProcessor: unsupported request");
    return Util::Response::fail("Unsupported request");
}

void RequestProcessor::registerHandler(const char* key, IRequestHandler* handler)
{
    std::lock_guard l(m_mutex);

    m_handlers.insert({ key, handler });

    m_log->write(Log::Level::Info, "RequestProcessor: registered handler %p for %s", handler, key);
}

void RequestProcessor::unregisterHandler(const char* key, IRequestHandler* handler)
{
    std::lock_guard l(m_mutex);

    auto range = m_handlers.equal_range(key);
    for (auto it = range.first; it != range.second; ++it)
    {
        if (it->second == handler)
        {
            m_handlers.erase(it);
            m_log->write(Log::Level::Info, "RequestProcessor: unregistered handler %p for %s", handler, key);
            return;
        }
    }
    
    m_log->write(Log::Level::Error, "RequestProcessor: no handlers registered for %s", key);
}

void RequestProcessor::startSession(uint32_t id)
{
    m_log->write(Log::Level::Info, "RequestProcessor: session %d started", id);

    std::lock_guard l(m_mutex);

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
    {
        it->second->startSession(id);
    }
}

void RequestProcessor::endSession(uint32_t id)
{
    std::lock_guard l(m_mutex);

    for (auto it = m_handlers.begin(); it != m_handlers.end(); ++it)
    {
        it->second->endSession(id);
    }

    m_log->write(Log::Level::Info, "RequestProcessor: session %d finished", id);
}

} // namespace Private {}

} // namespace Kes {}
