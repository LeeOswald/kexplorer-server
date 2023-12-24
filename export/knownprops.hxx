#pragma once

#include <export/property.hxx>


namespace Kes
{

namespace ExceptionProps
{


KESCOMMON_EXPORT void registerProperty(IPropertyInfo* pi);
KESCOMMON_EXPORT IPropertyInfo* lookupProperty(PropId id);
KESCOMMON_EXPORT void registerAll();


using DecodedError = PropertyInfo<std::string, KES_PROPID("decoded_error"), "Error message", PropertyFormatter<std::string>>;
using PosixErrorCode = PropertyInfo<int, KES_PROPID("posix_error_code"), "POSIX error code", PropertyFormatter<int>>;


} // ExceptionProps {}

} // namespace Kes {}
