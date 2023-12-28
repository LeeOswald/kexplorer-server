#pragma once

#include <kesrv/kesrv.hxx>


namespace Kes
{

namespace Util
{

KESRV_EXPORT std::string formatv(const char* format, va_list args);
KESRV_EXPORT std::string format(const char* format, ...);

} // namespace Util {}

} // namespace Kes {}

