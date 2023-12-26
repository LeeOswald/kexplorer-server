#include "globalcmdhandler.hxx"
#include "kexplorer-version.h"

#include <sstream>


namespace Kes
{

namespace Private
{

namespace
{

const char* const s_commands[] = 
{
    "stop",
    "version"
};

} // namespace {}

GlobalCmdHandler::~GlobalCmdHandler()
{
    for (auto cmd: s_commands)
    {
        m_rp->unregisterHandler(cmd, this);
    }
}

GlobalCmdHandler::GlobalCmdHandler(IRequestProcessor* rp, Condition& exitCondition, Log::ILog* log)
    : m_rp(rp)
    , m_exitCondition(exitCondition)
    , m_log(log)
{
    for (auto cmd: s_commands)
    {
        m_rp->registerHandler(cmd, this);
    }
}

bool GlobalCmdHandler::process(uint32_t sessionId, const char* key, const Json::Document& request, Json::Document& response)
{
    auto& a = response.GetAllocator();

    if (!std::strcmp(key, "stop"))
    {
        m_log->write(Log::Level::Info, "GlobalCmdHandler: [stop] command received");

        response.AddMember("status", Json::Value("success", a), a);

        m_exitCondition.set();

        return true;
    }

    if (!std::strcmp(key, "version"))
    {
        m_log->write(Log::Level::Info, "GlobalCmdHandler: [version] command received");
        std::ostringstream ss;
        ss << KES_APPLICATION_NAME << " " << KES_VERSION_STR << " " << KES_COPYRIGHT;

        response.AddMember("status", Json::Value("success", a), a);
        auto v = ss.str();
        response.AddMember("version", Json::Value(v.c_str(), v.length(), a), a);

        return true;
    }

    m_log->write(Log::Level::Error, "GlobalCmdHandler: unknown command [%s]", key);

    return false;
}

void GlobalCmdHandler::startSession(uint32_t id)
{

}

void GlobalCmdHandler::endSession(uint32_t id)
{

}

} // namespace Private {}

} // namespace Kes {}
