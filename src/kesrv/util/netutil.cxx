#include <kesrv/exception.hxx>
#include <kesrv/util/netutil.hxx>


namespace Kes
{

namespace Util
{

KESRV_EXPORT std::pair<std::string, uint16_t> splitAddress(const std::string& s)
{
    auto p0 = s.find_last_of('.');
    auto p1 = s.find_last_of(':');
    if ((p0 == s.npos) || (p1 == s.npos))
        throw Kes::Exception(KES_HERE(), "Invalid server address:port pair");

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

    uint16_t port = uint16_t(std::strtoul(portPart.c_str(), nullptr, 10));
    if (!port)
        throw Kes::Exception(KES_HERE(), "Invalid server port");

    return std::make_pair(std::move(addressPart), port);
}

KESRV_EXPORT boost::asio::ip::tcp::endpoint endpointFromString(const std::string& s)
{
    auto parts = splitAddress(s);
    auto address = boost::asio::ip::make_address(parts.first);

    return boost::asio::ip::tcp::endpoint(address, parts.second);
}


} // namespace Util {}

} // namespace Kes {}
