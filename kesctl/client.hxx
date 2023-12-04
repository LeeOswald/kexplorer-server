#pragma once

#include <export/platform.hxx>

#include <ostream>
#include <vector>

#include <boost/asio.hpp>


namespace Kesctl
{

class Client final
{
public:
    ~Client();
    explicit Client(boost::asio::io_context& io, const char* addr, uint16_t port, bool verbose, std::ostream& cout, std::ostream& cerr);

    Client(const Client&) = delete;
    Client& operator=(const Client&) = delete;

    Client(Client&&) = delete;
    Client& operator=(Client&&) = delete;

    std::string command(const std::string& cmd);

private:
    bool checkConnection() noexcept;
    void send(const std::string& data);
    std::string receive();

    boost::asio::io_context& m_io;
    bool m_verbose;
    std::ostream& m_cout;
    std::ostream& m_cerr;
    std::vector<boost::asio::ip::tcp::endpoint> m_endpoints;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
};


} // namespace Kesctl {}
