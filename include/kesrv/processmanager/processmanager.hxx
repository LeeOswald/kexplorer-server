#pragma once

#include <kesrv/log.hxx>
#include <kesrv/requestprocessor.hxx>
#include <kesrv/processmanager/procfs.hxx>

#include <mutex>
#include <unordered_map>


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
    
    bool process(uint32_t sessionId, const char* key, const PropertyBag& request, PropertyBag& response) override;
    void startSession(uint32_t id) override;
    void endSession(uint32_t id) override;

private:
    struct ProcessInfo
    {
        using Ptr = std::unique_ptr<ProcessInfo>;

        explicit ProcessInfo(uint32_t timestamp, ProcFs::Stat&& stat) noexcept
            : timestamp(timestamp)
            , stat(std::move(stat))
        {}

        ProcessInfo(const ProcessInfo&) = delete;
        ProcessInfo& operator=(const ProcessInfo&) = delete;

        PropertyBag serialize() const;

        uint32_t timestamp;
        bool newcomer = false;
        ProcFs::Stat stat;
        std::string comm;
        std::string exe;
        std::string cmdLine;
    };

    struct Session
    {
        using Ptr = std::unique_ptr<Session>;

        explicit Session(uint32_t id) noexcept
            : sessionId(id)
        {}

        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;

        uint32_t sessionId;
        uint32_t timestamp = 0;
        std::unordered_map<pid_t, ProcessInfo::Ptr> processes;
        std::vector<pid_t> removedPids;
    };

    bool process(Session* session, const char* key, const PropertyBag& request, PropertyBag& response);
    bool listProcesses(bool initial, Session* session, const PropertyBag& request, PropertyBag& response);
    ProcessInfo::Ptr readProcess(pid_t pid, uint32_t timestamp);
    void readProcesses(bool initial, Session* session);

    IRequestProcessor* m_rp;
    Log::ILog* m_log;
    ProcFs::ProcFs m_procFs;
    std::mutex m_mutex;
    std::unordered_map<uint32_t, Session::Ptr> m_sessions;
};


} // namespace Private {}

} // namespace Kes {}
