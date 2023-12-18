#pragma once

#include <export/platform.hxx>

#include <boost/asio.hpp>



namespace Kes
{

namespace Util
{

KESCOMMON_EXPORT std::pair<std::string, uint16_t> splitAddress(const std::string& s);
KESCOMMON_EXPORT boost::asio::ip::tcp::endpoint endpointFromString(const std::string& s);


} // namespace Util {}

} // namespace Kes {}
