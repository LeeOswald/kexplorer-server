#pragma once

#include <kesrv/property.hxx>

namespace Kes
{

namespace Request
{

using Id = int;

namespace Props
{

using Id = PropertyInfo<Request::Id, KES_PROPID("request.id"), "RequestId", PropertyFormatter<Request::Id>>;
using Command = PropertyInfo<std::string, KES_PROPID("request.request"), "Request", PropertyFormatter<std::string>>;

} // namespace Props {}

} // namespace Request {}


namespace Response
{

constexpr const char* const Success = "success";
constexpr const char* const Fail = "fail";

namespace Props
{

using Status = PropertyInfo<std::string, KES_PROPID("response.status"), "Status", PropertyFormatter<std::string>>;
using Reason = PropertyInfo<std::string, KES_PROPID("response.reason"), "Reason", PropertyFormatter<std::string>>;
using Version = PropertyInfo<std::string, KES_PROPID("response.srv_version"), "Server Version", PropertyFormatter<std::string>>;

} // namespace Props {}

} // namespace Response {}

} // namespace Kes {}

