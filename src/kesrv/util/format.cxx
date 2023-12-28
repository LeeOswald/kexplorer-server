#include <kesrv/util/format.hxx>


namespace Kes
{

namespace Util
{

KESRV_EXPORT std::string formatv(const char* format, va_list args)
{
    if (!format || !*format)
    {
        return std::string();
    }

    va_list args1;

    va_copy(args1, args);
    auto required = ::vsnprintf(nullptr, 0, format, args1);
    va_end(args1);

    va_list args2;
    va_copy(args2, args);

    std::string buffer;
    buffer.resize(required);

    ::vsnprintf(buffer.data(), required + 1, format, args2);

    va_end(args2);
    return buffer;
}

KESRV_EXPORT std::string format(const char* format, ...)
{
    if (!format || !*format)
    {
        return std::string();
    }

    va_list args;
    va_start(args, format);
    auto buffer = formatv(format, args);
    va_end(args);

    return buffer;
}


} // namespace Util {}

} // namespace Kes {}


