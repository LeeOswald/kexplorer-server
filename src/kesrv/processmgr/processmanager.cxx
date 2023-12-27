#include <kesrv/processmanager/processmanager.hxx>
#include <kesrv/processmanager/processprops.hxx>

namespace Kes
{

namespace Private
{

namespace
{

const char* const s_commands[] = 
{
    "list_processes",
    "diff_processes"
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
        return listProcesses(true, session, request, response);
    else if (!std::strcmp(key, "diff_processes"))
        return listProcesses(false, session, request, response);

    return false;
}

bool ProcessManager::listProcesses(bool initial, Session* session, const Json::Document& request, Json::Document& response)
{
    readProcesses(initial, session);

    auto& a = response.GetAllocator();

    // list existing/new processes
    {
        Json::Value jProcessArray(Json::kArrayType);
        
        for (auto& process: session->processes)
        {
            auto jProcess = process.second->serialize(response);
            jProcessArray.PushBack(std::move(jProcess), a);
        }

        response.AddMember("process_list", std::move(jProcessArray), a);
    }
    
    // list deleted processes
    if (!initial)
    {
        Json::Value jRemovedProcessArray(Json::kArrayType);

        for (auto pid: session->removedPids)
        {
            jRemovedProcessArray.PushBack(Json::Value(pid), a);
        }

        response.AddMember("removed_process_list", std::move(jRemovedProcessArray), a);
    }

    response.AddMember("status", Json::Value("success", a), a);
    return true;
}

void ProcessManager::readProcesses(bool initial, Session* session)
{
    ++session->timestamp;

    if (initial)
        session->processes.clear();

    session->removedPids.clear();

    auto pids = m_procFs.enumeratePids();
    for (auto pid: pids)
    {
        auto process = readProcess(pid, session->timestamp);
        if (!initial)
        {
            // check if this is a new process
            auto it = session->processes.find(pid);
            if (it == session->processes.end())
            {
                m_log->write(Log::Level::Info, "NEW process %d [%s]", pid, process->stat.comm.c_str());

                process->newcomer = true;
                session->processes.insert({ pid, std::move(process) });
            }
            else
            {
                // update the existing process
                std::swap(process, it->second);
            }
        }
        else
        {
            session->processes.insert({ pid, std::move(process) });
        }
    }

    if (!initial)
    {
        // detect deleted processes
        for (auto it = session->processes.begin(); it != session->processes.end(); ++it)
        {
            if (it->second->timestamp < session->timestamp)
            {
                m_log->write(Log::Level::Info, "DELETED process %d [%s]", it->first, it->second->stat.comm.c_str());

                session->removedPids.push_back(it->first);
                session->processes.erase(it);
            }
        }
    }
}

ProcessManager::ProcessInfo::Ptr ProcessManager::readProcess(pid_t pid, uint32_t timestamp)
{
    auto process = std::make_unique<ProcessInfo>(timestamp, m_procFs.readStat(pid));

    process->comm = m_procFs.readComm(pid);
    process->exe = m_procFs.readExePath(pid);
    process->cmdLine = m_procFs.readCmdLine(pid);

    return process;
}

Json::Value ProcessManager::ProcessInfo::serialize(Json::Document& doc)
{
    auto& a = doc.GetAllocator();

    Json::Value j(rapidjson::kObjectType);

    j.AddMember(Json::GenericStringRef(ProcessProps::Pid::idstr()), Json::Value(stat.pid), a);

    if (newcomer)
    {
        j.AddMember(Json::GenericStringRef(ProcessProps::Newcomer::idstr()), Json::Value(true), a);
    }
    
    if (!stat.valid)
    {
        j.AddMember(Json::GenericStringRef(ProcessProps::Error::idstr()), Json::Value(stat.error.c_str(), a), a);
    }
    else
    {    
        j.AddMember(Json::GenericStringRef(ProcessProps::PPid::idstr()), Json::Value(stat.ppid), a);
        j.AddMember(Json::GenericStringRef(ProcessProps::PGrp::idstr()), Json::Value(stat.pgrp), a);
        j.AddMember(Json::GenericStringRef(ProcessProps::Tpgid::idstr()), Json::Value(stat.tpgid), a);
        j.AddMember(Json::GenericStringRef(ProcessProps::Session::idstr()), Json::Value(stat.session), a);
        j.AddMember(Json::GenericStringRef(ProcessProps::Comm::idstr()), Json::Value(stat.comm.c_str(), a), a);
        j.AddMember(Json::GenericStringRef(ProcessProps::Ruid::idstr()), Json::Value(stat.ruid), a);

        if (!comm.empty())
            j.AddMember(Json::GenericStringRef(ProcessProps::StatComm::idstr()), Json::Value(comm.c_str(), a), a);

        if (!exe.empty())
            j.AddMember(Json::GenericStringRef(ProcessProps::Exe::idstr()), Json::Value(exe.c_str(), a), a);

        if (!cmdLine.empty())
            j.AddMember(Json::GenericStringRef(ProcessProps::CmdLine::idstr()), Json::Value(cmdLine.c_str(), a), a);
    }

    return j;
}

} // namespace Private {}    

} // namespace Kes {}
