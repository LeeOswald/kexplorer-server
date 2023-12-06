#include <export/util/request.hxx>


namespace Kes
{

namespace Util
{


namespace Request
{

std::string stop()
{
    return std::string("{\"request\":\"stop\"}");
}

} // namespace Request {}

namespace Response
{

std::string fail(const char* reason)
{
    return std::string("{\"status\":\"fail\",\"reason\"=\"") + std::string(reason) + std::string("\"}");
}


} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
