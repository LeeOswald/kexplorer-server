#pragma once

#include <export/platform.hxx>

#include <boost/asio.hpp>



namespace Kes
{

namespace Util
{

boost::asio::ip::tcp::endpoint endpointFromString(const std::string& s);


} // namespace Util {}

} // namespace Kes {}
