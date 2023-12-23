#pragma once

#include <export/property.hxx>


namespace Kes
{

namespace ExceptionProps
{

using DecodedError = PropertyInfo<std::string, KES_PROPID("decoded_error"), "Error message", PropertyFormatter<std::string>>;

using PosixErrorCode = PropertyInfo<int, KES_PROPID("posix_error_code"), "POSIX error code", PropertyFormatter<int>>;


inline bool format(const Property& prop, std::ostream& stream)
{
    switch (prop.id)
    {
    case DecodedError::Id::value: { DecodedError::Formatter f; f(prop, stream); return true; }
    case PosixErrorCode::Id::value: { PosixErrorCode::Formatter f; f(prop, stream); return true; }
    }

    return false;
}


} // ExceptionProps {}

} // namespace Kes {}
