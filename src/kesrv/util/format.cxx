#include <kesrv/util/format.hxx>


namespace Kes
{

namespace Util
{

KESRV_EXPORT std::string format(const char* format, ...)
{
    if (!format || !*format)
    {
        return std::string();
    }

    va_list args1;
    va_start(args1, format);
    auto required = ::vsnprintf(nullptr, 0, format, args1);
    va_end(args1);

    va_list args2;
    va_start(args2, format);

    std::string buffer;
    buffer.resize(required);

    ::vsnprintf(buffer.data(), required + 1, format, args2);

    va_end(args2);
    return buffer;
}


} // namespace Util {}

} // namespace Kes {}


