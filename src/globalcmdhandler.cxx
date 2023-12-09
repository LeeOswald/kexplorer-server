#include "globalcmdhandler.hxx"

#include <sstream>


namespace Kes
{

namespace Private
{

bool GlobalCmdHandler::process(const char* key, const Json::Document& request, Json::Document& response)
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


} // namespace Private {}

} // namespace Kes {}
