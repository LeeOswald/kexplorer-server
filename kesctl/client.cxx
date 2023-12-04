#include "client.hxx"

#include <sstream>


namespace Kesctl
{

Client::~Client()
{
    if (m_socket && m_socket->is_open())
    {
        boost::system::error_code ec;
        m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send);
        m_socket->close();
    }
}

Client::Client(boost::asio::io_context& io, const char* addr, uint16_t port, bool verbose, std::ostream& cout, std::ostream& cerr)
    : m_io(io)
    , m_verbose(verbose)
    , m_cout(cout)
    , m_cerr(cerr)
{
    // resolve server address
    boost::asio::ip::tcp::resolver resolver(m_io);
    boost::asio::ip::tcp::resolver::query query(addr, "");
    boost::system::error_code ec;
    auto results = resolver.resolve(query, ec);
    if (ec)
    {
        throw std::runtime_error(std::string("Failed to ") + ec.message());
    }

    for (auto& r : results)
    {
        if (m_verbose)
            m_cout << addr << " -> " << r.endpoint().address().to_string() << "\n";

        m_endpoints.push_back(boost::asio::ip::tcp::endpoint(r.endpoint().address(), port));
    }
}

bool Client::checkConnection() noexcept
{
    if (m_socket)
    {
        if (m_socket->is_open())
            return true;
    }

    for (auto& ep: m_endpoints)
    {
        if (m_verbose)
            m_cout << "Connecting to " << ep.address().to_string() << "...\n";

        m_socket.reset(new boost::asio::ip::tcp::socket(m_io));

        boost::system::error_code ec;
        m_socket->connect(ep, ec);
        if (!ec)
        {
            if (m_verbose)
                m_cout << "Connected to " << ep.address().to_string() << "\n";

            return true;
        }

        m_socket.reset();
    }

    return false;
}

std::string Client::command(const std::string& cmd)
{
    if (!checkConnection())
    {
        throw std::runtime_error("Unable to connect to the server");
    }

    if (m_verbose)
        m_cout << cmd << "\n";

    send(cmd);

    return receive();
}

void Client::send(const std::string& data)
{
    auto p = data.data();
    auto size = data.length();

    while (size)
    {
        if (m_verbose)
            m_cout << "Sending " << size << " bytes\n";

        boost::system::error_code ec;
        auto written = m_socket->write_some(boost::asio::const_buffer(p, size), ec);

        if (ec)
        {
            throw std::runtime_error(std::string("Failed to send the data: ") + ec.message());
        }

        if (m_verbose)
            m_cout << "    Sent " << written << " bytes\n";

        assert(size >= written);
        size -= written;
        p += written;
    }
}

std::string Client::receive()
{
    std::ostringstream result;
    std::vector<char> buffer(65536);

    do
    {
        if (m_verbose)
            m_cout << "Receiving...\n";

        boost::system::error_code ec;
        auto received = m_socket->read_some(boost::asio::buffer(buffer), ec);

        if (ec)
        {
            m_cerr << "Failed to receive the data: " << ec.message() << "\n";
            break;
        }

        if (m_verbose)
            m_cout << "    Received " << received << " bytes\n";

        if (received)
            result << std::string_view(buffer.data(), received);
        else
            break;

    } while (true);

    return result.str();
}

} // namespace Kesctl {}
