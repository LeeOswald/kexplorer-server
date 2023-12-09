#include <export/util/request.hxx>


namespace Kes
{

namespace Util
{


namespace Request
{

std::string simple(const char* command)
{
    Json::Document doc;
    auto a = doc.GetAllocator();

    doc.SetObject();
    doc.AddMember("request", Json::Value(command, a), a);

    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);
    doc.Accept(writer);

    return sb.GetString();
}

} // namespace Request {}

namespace Response
{

std::string fail(const char* reason)
{
    Json::Document doc;
    auto a = doc.GetAllocator();

    doc.SetObject();
    doc.AddMember("status", Json::Value("fail", a), a);
    doc.AddMember("reason", Json::Value(reason, a), a);

    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);
    doc.Accept(writer);

    return sb.GetString();
}


} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
