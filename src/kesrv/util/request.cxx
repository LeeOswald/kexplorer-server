#include <kesrv/util/request.hxx>


namespace Kes
{

namespace Util
{


namespace Request
{

KESRV_EXPORT std::string simple(const char* command)
{
    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);
    
    writer.StartObject();
    writer.Key("request");
    writer.String(command);
    writer.EndObject();

    return sb.GetString();
}

} // namespace Request {}

namespace Response
{

KESRV_EXPORT std::string fail(const char* reason)
{
    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);
    
    writer.StartObject();
    writer.Key("status");
    writer.String("fail");
    writer.Key("reason");
    writer.String(reason);

    return sb.GetString();
}


} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
