#pragma once

#include <export/condition.hxx>
#include <export/json.hxx>
#include <export/log.hxx>


namespace Kes
{

namespace Private
{


class RequestProcessor final
{
public:
    explicit RequestProcessor(Condition* exitCondition, Log::ILog* log)
        : m_exitCondition(exitCondition)
        , m_log(log)
    {

    }

    RequestProcessor(const RequestProcessor&) = delete;
    RequestProcessor& operator=(const RequestProcessor&) = delete;

    RequestProcessor(RequestProcessor&&) = delete;
    RequestProcessor& operator=(RequestProcessor&&) = delete;

    std::string process(const char* request, size_t length);

private:
    Condition* m_exitCondition;
    Log::ILog* m_log;
};


} // namespace Private {}

} // namespace Kes {}
