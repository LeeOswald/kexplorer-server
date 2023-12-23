#include <kesrv/processmgr/processmanager.hxx>

namespace Kes
{

namespace Private
{

namespace
{

const char* const s_commands[] = 
{
    "list_processes",
};

} // namespace {}

ProcessManager::~ProcessManager()
{
    for (auto cmd: s_commands)
    {
        m_rp->unregisterHandler(cmd, this);
    }
}

ProcessManager::ProcessManager(IRequestProcessor* rp, Log::ILog* log)
    : m_rp(rp)
    , m_log(log)
    , m_procFs(log)
{
    for (auto cmd: s_commands)
    {
        m_rp->registerHandler(cmd, this);
    }
}

bool ProcessManager::process(uint32_t sessionId, const char* key, const Json::Document& request, Json::Document& response)
{

    m_log->write(Log::Level::Error, "ProcessManager: unknown command [%s]", key);
    return false;
}

void ProcessManager::startSession(uint32_t id)
{

}

void ProcessManager::endSession(uint32_t id)
{

}

} // namespace Private {}    

} // namespace Kes {}