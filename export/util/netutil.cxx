#include <export/util/netutil.hxx>


namespace Kes
{

namespace Util
{


boost::asio::ip::tcp::endpoint endpointFromString(const std::string& s)
{
    auto p0 = s.find_last_of('.');
    auto p1 = s.find_last_of(':');
    if ((p0 == s.npos) || (p1 == s.npos))
        throw std::invalid_argument("Invalid server address:port pair");

    std::string addressPart, portPart;
    if (p1 > p0)
    {
        // IPv4 127.0.0.1:8080
        addressPart = s.substr(0, p1);
        portPart = s.substr(p1 + 1);
    }
    else if (p0 > p1)
    {
        // IPv6 ::1.8080
        addressPart = s.substr(0, p0);
        portPart = s.substr(p0 + 1);
    }

    auto address = boost::asio::ip::make_address(addressPart);
    uint16_t port = uint16_t(std::strtoul(portPart.c_str(), nullptr, 10));
    if (!port)
        throw std::invalid_argument("Invalid server port");

    return boost::asio::ip::tcp::endpoint(address, port);
}


} // namespace Util {}

} // namespace Kes {}
