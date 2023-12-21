#include <export/util/posixerror.hxx>

namespace Kes
{

namespace Util
{
    
std::string KESCOMMON_EXPORT posixErrorToString(int e)
{
    constexpr size_t required = 256;
    char result[required];
    
    auto s = ::strerror_r(e, result, required); // 's' may be or not be the same as 'result'

    return std::string(s);
}
    
    
} // namespace Util {}

} // namespace Kes {}
