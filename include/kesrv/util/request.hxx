#pragma once

#include <kesrv/json.hxx>

namespace Kes
{

namespace Util
{

namespace Request
{

KESRV_EXPORT std::string simple(const char* command);

} // namespace Request {}


namespace Response
{

KESRV_EXPORT std::string fail(const char* reason);


} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
