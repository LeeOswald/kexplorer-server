#include "globalcmdhandler.hxx"

#include <sstream>


namespace Kes
{

namespace Private
{

GlobalCmdHandler::~GlobalCmdHandler()
{
    m_rp->unregisterHandler("stop", this);
    m_rp->unregisterHandler("version", this);
}

GlobalCmdHandler::GlobalCmdHandler(IRequestProcessor* rp, Condition& exitCondition, Log::ILog* log)
        : m_rp(rp)
        , m_exitCondition(exitCondition)
        , m_log(log)
{
    m_rp->registerHandler("stop", this);
    m_rp->registerHandler("version", this);
}

bool GlobalCmdHandler::process(uint32_t sessionId, const char* key, const Json::Document& request, Json::Document& response)
{
    if (!std::strcmp(key, "stop"))
    {
        m_log->write(Log::Level::Info, "GlobalCmdHandler: [stop] command received");

        response.AddMember("status", Json::Value("success", response.GetAllocator()), response.GetAllocator());

        m_exitCondition.set();

        return true;
    }

    if (!std::strcmp(key, "version"))
    {
        m_log->write(Log::Level::Info, "GlobalCmdHandler: [version] command received");
        std::ostringstream ss;
        ss << KES_APPLICATION_NAME << " " << KES_VERSION_STR << " " << KES_COPYRIGHT;

        response.AddMember("status", Json::Value("success", response.GetAllocator()), response.GetAllocator());
        auto v = ss.str();
        response.AddMember("version", Json::Value(v.c_str(), v.length(), response.GetAllocator()), response.GetAllocator());

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
