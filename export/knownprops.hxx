#pragma once

#include <export/property.hxx>


namespace Kes
{

namespace Props
{

using PosixErrorCodeTraits = PropertyTypeTraits<int, KES_PROPID("posix_error_code")>;
using PosixErrorCode = PropertyInfo<PosixErrorCodeTraits, "POSIX error code", PosixErrorCodeTraits>;

} // namespace Props {}

} // namespace Kes {}
