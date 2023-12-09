#pragma once

#include <export/condition.hxx>
#include <export/log.hxx>
#include <kesrv/kesrv.hxx>


namespace Kes
{

namespace Private
{

class GlobalCmdHandler final
    : public IRequestHandler
{
public:
    ~GlobalCmdHandler()
    {
    }
    
    explicit GlobalCmdHandler(Condition& exitCondition, Log::ILog* log)
        : m_exitCondition(exitCondition)
        , m_log(log)
    {
    }

    GlobalCmdHandler(const GlobalCmdHandler&) = delete;
    GlobalCmdHandler& operator=(const GlobalCmdHandler&) = delete;

    GlobalCmdHandler(GlobalCmdHandler&&) = delete;
    GlobalCmdHandler& operator=(GlobalCmdHandler&&) = delete;
    
    bool process(const char* key, const Json::Document& request, Json::Document& response) override;

private:
    Condition& m_exitCondition;
    Log::ILog* m_log;
};


} // namespace Private {}

} // namespace Kes {}