#pragma once

#include <kesrv/log.hxx>
#include <kesrv/requestprocessor.hxx>

#include <mutex>
#include <unordered_map>


namespace Kes
{

namespace Private
{


class RequestProcessor final
    : public IRequestProcessor
{
public:
    explicit RequestProcessor(Log::ILog* log)
        : m_log(log)
    {
    }

    ~RequestProcessor()
    {
    }

    RequestProcessor(const RequestProcessor&) = delete;
    RequestProcessor& operator=(const RequestProcessor&) = delete;

    RequestProcessor(RequestProcessor&&) = delete;
    RequestProcessor& operator=(RequestProcessor&&) = delete;

    std::string process(uint32_t sessionId, char* request, size_t length) override;
    void registerHandler(const char* key, IRequestHandler* handler) override;
    void unregisterHandler(const char* key, IRequestHandler* handler) override;
    void startSession(uint32_t id) override;
    void endSession(uint32_t id) override;

private:
     Log::ILog* m_log;
     std::mutex m_mutex;
     std::unordered_multimap<std::string, IRequestHandler*> m_handlers;
};


} // namespace Private {}

} // namespace Kes {}
