#include "requestprocessor.hxx"

#include <kesrv/exception.hxx>
#include <kesrv/util/request.hxx>


namespace Kes
{

namespace Private
{

class JsonErrorHandler :
    public Kes::IPropertyErrorHandler
{
public:
    explicit JsonErrorHandler(Log::ILog* log) noexcept
        : m_log(log)
    {}

    Kes::CallbackResult handle(Kes::SourceLocation where, const std::string& message) noexcept override
    {
        m_errors = true;
        m_message = message;

        m_log->write(Kes::Log::Level::Warning, "%s", message.c_str());
        return Kes::CallbackResult::Continue;
    }

    [[nodiscard]] bool error() const noexcept
    {
        return m_errors;
    }

    [[nodiscard]] const std::string& message() const noexcept
    {
        return m_message;
    }

private:
    Log::ILog* m_log;
    bool m_errors = false;
    std::string m_message;
};


std::string RequestProcessor::process(uint32_t sessionId, char* request, [[maybe_unused]] size_t length)
{
    try
    {
        LogDebug(m_log, "\n-> %s\n", request);

        JsonErrorHandler eh(m_log);
        auto parsedRequest = propertyBagFromJson(request, &eh);
        
        if (!parsedRequest.isTable())
        {
            m_log->write(Log::Level::Error, "RequestProcessor: request is not a JSON object");
            return Util::Response::fail("Not a JSON object");
        }

        auto requestKeyIt = parsedRequest.table().find(Kes::Util::Request::Props::Command::idstr());
        if (requestKeyIt == parsedRequest.table().end())
        {
            m_log->write(Log::Level::Error, "RequestProcessor: \'request\' key not found");
            return Util::Response::fail("Unsupported request");
        }

        if (!requestKeyIt->second->isProperty())
        {
            m_log->write(Log::Level::Error, "RequestProcessor: ill-formed reuest");
            return Util::Response::fail("Ill-formed request");
        }

        auto key = std::any_cast<std::string>(requestKeyIt->second->property().value);

        {
            PropertyBag response{std::string(), PropertyBag::Table()};

            bool handlerFound = false;
            {
                std::lock_guard l(m_mutex);

                auto range = m_handlers.equal_range(key);
                for (auto it = range.first; it != range.second; ++it)
                {
                    auto handler = it->second;
                    handlerFound = handler->process(sessionId, key.c_str(), parsedRequest, response);
                }
            }

            if (!handlerFound)
            {
                m_log->write(Log::Level::Error, "RequestProcessor: unsupported request");
                return Util::Response::fail("Unsupported request");
            }

            auto out = propertyBagToJson(response);

            LogDebug(m_log, "\n<- %s\n", out.c_str());

            return out;
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
