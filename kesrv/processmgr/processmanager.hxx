#pragma once

#include <export/log.hxx>
#include <kesrv/kesrv.hxx>
#include <kesrv/processmgr/procfs.hxx>

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
    
    bool process(uint32_t sessionId, const char* key, const Json::Document& request, Json::Document& response) override;
    void startSession(uint32_t id) override;
    void endSession(uint32_t id) override;

private:
    struct ProcessInfo
    {
        using Ptr = std::unique_ptr<ProcessInfo>;

        explicit ProcessInfo(ProcFs::Stat&& stat) noexcept
            : stat(std::move(stat))
        {}

        ProcessInfo(const ProcessInfo&) = delete;
        ProcessInfo& operator=(const ProcessInfo&) = delete;

        Json::Value serialize(Json::Document& doc);

        ProcFs::Stat stat;
        std::optional<std::string> comm;
        std::optional<std::string> exe;
        std::optional<std::string> cmdLine;
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
        std::unordered_map<pid_t, ProcessInfo::Ptr> processes;
    };

    bool process(Session* session, const char* key, const Json::Document& request, Json::Document& response);
    bool listProcesses(Session* session, const Json::Document& request, Json::Document& response);
    

    IRequestProcessor* m_rp;
    Log::ILog* m_log;
    ProcFs::ProcFs m_procFs;
    std::mutex m_mutex;
    std::unordered_map<uint32_t, Session::Ptr> m_sessions;
};


} // namespace Private {}

} // namespace Kes {}