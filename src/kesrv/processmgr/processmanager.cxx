#include <kesrv/processmanager/processmanager.hxx>
#include <kesrv/processmanager/processprops.hxx>
#include <kesrv/util/request.hxx>

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

bool ProcessManager::process(uint32_t sessionId, const char* key, const PropertyBag& request, PropertyBag& response)
{
    assert(request.isTable());
    assert(response.isTable());

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

    auto it = m_sessions.find(id);
    if (it != m_sessions.end())
    {
        LogDebug(m_log, "Session %d already exists", id);
        return;
    }

    m_sessions.insert({ id, std::make_unique<Session>(id) });
}

void ProcessManager::endSession(uint32_t id)
{
    std::lock_guard l(m_mutex);

    auto it = m_sessions.find(id);
    if (it != m_sessions.end())
    {
        m_sessions.erase(it);
    }
}

bool ProcessManager::process(Session* session, const char* key, const PropertyBag& request, PropertyBag& response)
{
    if (!std::strcmp(key, "list_processes"))
        return listProcesses(true, session, request, response);
    else if (!std::strcmp(key, "diff_processes"))
        return listProcesses(false, session, request, response);

    return false;
}

bool ProcessManager::listProcesses(bool initial, Session* session, const PropertyBag& request, PropertyBag& response)
{
    readProcesses(initial, session);

    // list existing/new processes
    {
        PropertyBag processArray{Kes::ProcessProps::ProcessList::idstr(), PropertyBag::Array()};
        
        for (auto& process: session->processes)
        {
            auto jProcess = process.second->serialize();
            Util::addToArray<Kes::ProcessProps::Process>(processArray, std::move(jProcess));
        }

        Util::addToTable<Kes::ProcessProps::ProcessList>(response, std::move(processArray));
    }
    
    // list deleted processes
    if (!initial)
    {
        PropertyBag processArray{Kes::ProcessProps::ProcessList::idstr(), PropertyBag::Array()};
        
        for (auto pid: session->removedPids)
        {
            Util::addToArray<Kes::ProcessProps::DeletedProcess>(processArray, int(pid));
        }

        Util::addToTable<Kes::ProcessProps::DeletedProcessList>(response, std::move(processArray));
    }

    Util::addToTable<Kes::Util::Response::Props::Status>(response, std::string(Util::Response::Success));
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

PropertyBag ProcessManager::ProcessInfo::serialize() const
{
    PropertyBag table{std::string(), PropertyBag::Table()};

    Util::addToTable<ProcessProps::Pid>(table, int(stat.pid));

    if (newcomer)
    {
        Util::addToTable<ProcessProps::Newcomer>(table, true);
    }
    
    if (!stat.valid)
    {
        Util::addToTable<ProcessProps::Error>(table, stat.error);
    }
    else
    {    
        Util::addToTable<ProcessProps::PPid>(table, int(stat.ppid));
        Util::addToTable<ProcessProps::PGrp>(table, int(stat.pgrp));
        Util::addToTable<ProcessProps::Tpgid>(table, int(stat.tpgid));
        Util::addToTable<ProcessProps::Session>(table, int(stat.session));
        Util::addToTable<ProcessProps::Comm>(table, comm);
        Util::addToTable<ProcessProps::Ruid>(table, int(stat.ruid));

        if (!comm.empty())
            Util::addToTable<ProcessProps::StatComm>(table, comm);

        if (!exe.empty())
            Util::addToTable<ProcessProps::Exe>(table, exe);

        if (!cmdLine.empty())
            Util::addToTable<ProcessProps::CmdLine>(table, cmdLine);
    }

    return table;
}

} // namespace Private {}    

} // namespace Kes {}
