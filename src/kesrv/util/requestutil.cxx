#include <kesrv/json.hxx>
#include <kesrv/knownprops.hxx>
#include <kesrv/request.hxx>
#include <kesrv/util/requestutil.hxx>


namespace Kes
{

namespace Util
{

namespace Request
{

KESRV_EXPORT std::string simple(Kes::Request::Id id, const char* command)
{
    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);
    
    writer.StartObject();
    
    writer.Key(Kes::Request::Props::Id::idstr());
    writer.Int(id);

    writer.Key(Kes::Request::Props::Command::idstr());
    writer.String(command);
    
    writer.EndObject();

    return sb.GetString();
}

namespace Props
{

namespace Private
{

void registerAll()
{
    Kes::registerProperty(new PropertyInfoWrapper<Kes::Request::Props::Id>);
    Kes::registerProperty(new PropertyInfoWrapper<Kes::Request::Props::Command>);
}

} // namespace Private {}

} // namespace Props {}

} // namespace Request {}

namespace Response
{

KESRV_EXPORT std::string fail(Kes::Request::Id id, const char* reason)
{
    Json::StringBuffer sb;
    Json::Writer<Json::StringBuffer> writer(sb);
    
    writer.StartObject();

    writer.Key(Kes::Request::Props::Id::idstr());
    writer.Int(id);
    
    writer.Key(Kes::Response::Props::Status::idstr());
    writer.String(Kes::Response::Fail);
    
    writer.Key(Kes::Response::Props::Reason::idstr());
    writer.String(reason);

    writer.EndObject();

    return sb.GetString();
}

namespace Props
{

namespace Private
{

void registerAll()
{
    Kes::registerProperty(new PropertyInfoWrapper<Kes::Response::Props::Status>);
    Kes::registerProperty(new PropertyInfoWrapper<Kes::Response::Props::Reason>);
    Kes::registerProperty(new PropertyInfoWrapper<Kes::Response::Props::Version>);
}

} // namespace Private {}

} // namespace Props {}

} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
