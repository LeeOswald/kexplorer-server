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
    ~GlobalCmdHandler();
    explicit GlobalCmdHandler(IRequestProcessor* rp, Condition& exitCondition, Log::ILog* log);

    GlobalCmdHandler(const GlobalCmdHandler&) = delete;
    GlobalCmdHandler& operator=(const GlobalCmdHandler&) = delete;

    GlobalCmdHandler(GlobalCmdHandler&&) = delete;
    GlobalCmdHandler& operator=(GlobalCmdHandler&&) = delete;
    
    bool process(uint32_t sessionId, const char* key, const Json::Document& request, Json::Document& response) override;
    void startSession(uint32_t id) override;
    void endSession(uint32_t id) override;

private:
    IRequestProcessor* m_rp;
    Condition& m_exitCondition;
    Log::ILog* m_log;
};


} // namespace Private {}

} // namespace Kes {}
