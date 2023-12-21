#pragma once

#include <export/property.hxx>


namespace Kes
{

namespace Props
{

using DecodedError = PropertyInfo<std::string, KES_PROPID("decoded_error"), "Error message">;

using PosixErrorCode = PropertyInfo<int, KES_PROPID("posix_error_code"), "POSIX error code">;

} // namespace Props {}

} // namespace Kes {}
