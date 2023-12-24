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
    std::lock_guard l(m_mutex);

    auto it = m_sessions.find(sessionId);
    assert(it != m_sessions.end());
    if (it == m_sessions.end())
        return false;

    if (process(it->second.get(), key, request, response))
        return true;

    m_log->write(Log::Level::Error, "ProcessManager: unknown command [%s]", key);
    return false;
}

void ProcessManager::startSession(uint32_t id)
{
    std::lock_guard l(m_mutex);

    [[maybe_unused]] auto ret = m_sessions.insert({ id, std::make_unique<Session>(id) });
    assert(ret.second);
}

void ProcessManager::endSession(uint32_t id)
{
    std::lock_guard l(m_mutex);

    auto it = m_sessions.find(id);
    assert(it != m_sessions.end());
    if (it != m_sessions.end())
    {
        m_sessions.erase(it);
    }
}

bool ProcessManager::process(Session* session, const char* key, const Json::Document& request, Json::Document& response)
{
    if (!std::strcmp(key, "list_processes"))
        return listProcesses(session, request, response);

    return false;
}

bool ProcessManager::listProcesses(Session* session, const Json::Document& request, Json::Document& response)
{
    auto& a = response.GetAllocator();

    Json::Value jProcessArray(Json::kArrayType);

    session->processes.clear();

    auto pids = m_procFs.enumeratePids();
    for (auto pid: pids)
    {
        auto r = session->processes.insert({ pid, std::make_unique<ProcessInfo>(m_procFs.readStat(pid)) });
        auto info = r.first->second.get();

        info->comm = m_procFs.readComm(pid);
        info->exe = m_procFs.readExePath(pid);
        info->cmdLine = m_procFs.readCmdLine(pid);

        auto jProcess = info->serialize(response);
        jProcessArray.PushBack(std::move(jProcess), a);
    }

    response.AddMember("process_list", std::move(jProcessArray), a);

    response.AddMember("status", Json::Value("success", a), a);
    return true;
}

Json::Value ProcessManager::ProcessInfo::serialize(Json::Document& doc)
{
    auto& a = doc.GetAllocator();

    Json::Value j(rapidjson::kObjectType);

    j.AddMember("pid", Json::Value(stat.pid), a);
    
    if (!stat.valid)
    {
        j.AddMember("error", Json::Value(stat.error.c_str(), a), a);
    }
    else
    {    
        j.AddMember("ppid", Json::Value(stat.ppid), a);
        j.AddMember("pgrp", Json::Value(stat.pgrp), a);
        j.AddMember("tpgid", Json::Value(stat.tpgid), a);
        j.AddMember("session", Json::Value(stat.session), a);
        j.AddMember("comm", Json::Value(stat.comm.c_str(), a), a);
        j.AddMember("ruid", Json::Value(stat.ruid), a);

        if (comm)
            j.AddMember("full_comm", Json::Value(comm->c_str(), a), a);

        if (exe)
            j.AddMember("exe", Json::Value(exe->c_str(), a), a);

        if (cmdLine)
            j.AddMember("cmdLine", Json::Value(cmdLine->c_str(), a), a);
    }

    return j;
}

} // namespace Private {}    

} // namespace Kes {}