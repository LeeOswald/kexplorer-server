#pragma once

#include <kesrv/json.hxx>
#include <kesrv/property.hxx>

namespace Kes
{

namespace Util
{

namespace Request
{

KESRV_EXPORT std::string simple(const char* command);

namespace Props
{

using Command = PropertyInfo<std::string, KES_PROPID("request.request"), "Request", PropertyFormatter<std::string>>;

} // namespace Props {}

namespace Private
{

void registerAll();

} // namespace Private {}

} // namespace Request {}


namespace Response
{

KESRV_EXPORT std::string fail(const char* reason);

namespace Props
{

using Status = PropertyInfo<std::string, KES_PROPID("response.status"), "Status", PropertyFormatter<std::string>>;
using Reason = PropertyInfo<std::string, KES_PROPID("response.reason"), "Reason", PropertyFormatter<std::string>>;

} // namespace Props {}

namespace Private
{

void registerAll();

} // namespace Private {}

} // namespace Response {}

} // namespace Util {}

} // namespace Kes {}
