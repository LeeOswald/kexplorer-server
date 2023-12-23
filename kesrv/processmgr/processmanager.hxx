#pragma once

#include <export/log.hxx>
#include <kesrv/kesrv.hxx>
#include <kesrv/processmgr/procfs.hxx>

namespace Kes
{

namespace Private
{

class KESRV_EXPORT ProcessManager final
    : public IRequestHandler
{
public:
    ~ProcessManager();
    explicit ProcessManager(IRequestProcessor* rp, Log::ILog* log);

    ProcessManager(const ProcessManager&) = delete;
    ProcessManager& operator=(const ProcessManager&) = delete;

    ProcessManager(ProcessManager&&) = delete;
    ProcessManager& operator=(ProcessManager&&) = delete;
    
    bool process(uint32_t sessionId, const char* key, const Json::Document& request, Json::Document& response) override;
    void startSession(uint32_t id) override;
    void endSession(uint32_t id) override;

private:
    IRequestProcessor* m_rp;
    Log::ILog* m_log;
    ProcFs::ProcFs m_procFs;
};


} // namespace Private {}

} // namespace Kes {}