#include <kesrv/json.hxx>
#include <kesrv/knownprops.hxx>
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
    writer.Key(Props::Command::idstr());
    writer.String(command);
    writer.EndObject();

    return sb.GetString();
}


namespace Private
{

void registerAll()
{
    Kes::registerProperty(new PropertyInfoWrapper<Props::Command>);
}

} // namespace Private {}

} // namespace Request {}

namespace Response
{

KESRV_EXPORT std::string fail(const char* reason)
{
    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);
    
    writer.StartObject();
    writer.Key(Props::Status::idstr());
    writer.String(Fail);
    writer.Key(Props::Reason::idstr());
    writer.String(reason);

    return sb.GetString();
}

namespace Private
{

void registerAll()
{
    Kes::registerProperty(new PropertyInfoWrapper<Props::Status>);
    Kes::registerProperty(new PropertyInfoWrapper<Props::Reason>);
    Kes::registerProperty(new PropertyInfoWrapper<Props::Version>);
}

} // namespace Private {}


} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
