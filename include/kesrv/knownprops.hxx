#pragma once

#include <kesrv/property.hxx>


namespace Kes
{

KESRV_EXPORT void registerProperty(IPropertyInfo* pi);
KESRV_EXPORT IPropertyInfo* lookupProperty(PropId id);
KESRV_EXPORT IPropertyInfo* lookupProperty(const char* id);

} // namespace Kes {}
