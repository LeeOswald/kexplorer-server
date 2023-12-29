#pragma once

#include <kesrv/kesrv.hxx>


namespace Kes
{

namespace Util
{

KESRV_EXPORT std::pair<std::string, uint16_t> splitAddress(const std::string& s);
KESRV_EXPORT boost::asio::ip::tcp::endpoint endpointFromString(const std::string& s);


} // namespace Util {}

} // namespace Kes {}
