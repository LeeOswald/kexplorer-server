#pragma once

#include <export/json.hxx>


namespace Kes
{

namespace Util
{

namespace Request
{

std::string simple(const char* command);

} // namespace Request {}


namespace Response
{

std::string fail(const char* reason);


} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
