#pragma once

#include <kesrv/property.hxx>


namespace Kes
{

namespace ExceptionProps
{


KESRV_EXPORT void registerProperty(IPropertyInfo* pi);
KESRV_EXPORT IPropertyInfo* lookupProperty(PropId id);

namespace Private
{

void registerAll();

} // namespace Private {}


using DecodedError = PropertyInfo<std::string, KES_PROPID("decoded_error"), "Error message", PropertyFormatter<std::string>>;
using PosixErrorCode = PropertyInfo<int, KES_PROPID("posix_error_code"), "POSIX error code", PropertyFormatter<int>>;


} // ExceptionProps {}

} // namespace Kes {}
