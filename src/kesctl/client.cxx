#include "client.hxx"

#include <kesrv/exception.hxx>

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
    , m_bufferIn(kBufferSize, kBufferLimit)
{
    // resolve server address
    boost::asio::ip::tcp::resolver resolver(m_io);
    boost::asio::ip::tcp::resolver::query query(addr, "");
    boost::system::error_code ec;
    auto results = resolver.resolve(query, ec);
    if (ec)
    {
        throw Kes::Exception(KES_HERE(), std::string("Failed to resolve the server address: ") + ec.message());
    }

    for (auto& r : results)
    {
        if (m_verbose)
            m_cout << addr << " -> " << r.endpoint().address().to_string() << "\n";

        m_endpoints.push_back(boost::asio::ip::tcp::endpoint(r.endpoint().address(), port));
    }
}

void Client::stop() noexcept
{
    m_stop = true;
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

void Client::command(const std::string& cmd)
{
    if (m_stop)
        return;

    reset();

    if (!checkConnection())
    {
        throw Kes::Exception(KES_HERE(), "Unable to connect to the server");
    }

    if (m_verbose)
        m_cout << cmd << "\n";

    write(std::make_shared<std::string>(std::move(cmd)));
}

void Client::write(std::shared_ptr<std::string> data) noexcept
{
    if (m_verbose)
        m_cout << "Sending " << data->size() << " bytes\n";

    boost::asio::async_write(
        *m_socket,
        boost::asio::const_buffer(data->data(), data->size()),
        [this, data](const boost::system::error_code& ec, size_t transferred)
        {
            this->onWrite(ec, transferred, data);
        }
    );
}

void Client::onWrite(const boost::system::error_code& ec, size_t transferred, std::shared_ptr<std::string> data) noexcept
{
    if (ec)
    {
        m_cerr << "write() failed: " << ec.message() << "\n";
    }
    else
    {
        if (m_verbose)
            m_cout << "    Sent " << transferred << " bytes\n";

        if (m_stop)
            return;

        read(Kes::Util::ReadBuffer::Ptr());
    }
}

void Client::read(Kes::Util::ReadBuffer::Ptr buffer) noexcept
{
    try
    {
        if (!buffer)
        {
            buffer = Kes::Util::ReadBuffer::create(kBufferSize);
        }

        m_socket->async_read_some(
            boost::asio::buffer(buffer->data(buffer->w_index()), buffer->size()),
            [this, buffer](const boost::system::error_code& ec, size_t transferred)
            {
                this->onRead(ec, transferred, buffer);
            }
        );

    }
    catch (std::exception& e)
    {
        m_cerr << "Failed to receive data: " << e.what() << "\n";
    }
}

void Client::onRead(const boost::system::error_code& ec, size_t transferred, Kes::Util::ReadBuffer::Ptr buffer) noexcept
{
    if (ec)
    {
        m_cerr << "read() failed: " << ec.message() << "\n";
    }
    else
    {
        if (m_verbose)
            m_cout << "Received " << transferred << " bytes\n";

        buffer->swap();

        if (!process(buffer->data(buffer->r_index()), transferred)) // want more data
            read(buffer);
        else
            m_io.stop();
    }
}

bool Client::process(const char* data, size_t size) noexcept
{
    try
    {
        auto posPrev = m_bufferIn.used();

        if (!m_bufferIn.push(data, size))
            throw Kes::Exception(KES_HERE(), "Packet size exceeds limit");

        auto posCur = m_bufferIn.used();

        if (posCur == posPrev)
        {
            // nothing to process, request moar data
            return false;
        }

        auto cur = m_bufferIn.data() + posPrev;
        auto end = m_bufferIn.data() + posCur;
        while (cur < end)
        {
            if (*cur == '{')
            {
                ++m_jsonDepth;
                if (m_jsonDepth > m_jsonDepthMax)
                    m_jsonDepthMax = m_jsonDepth;
            }
            else if (*cur == '}')
            {
                if (m_jsonDepth == 0)
                    throw Kes::Exception(KES_HERE(), "Invalid JSON");

                --m_jsonDepth;
            }

            ++cur;
        }

        if (m_jsonDepth == 0)
        {
            // JSON complete
            if (m_jsonDepthMax == 0)
            {
                // nothing to process, request moar data
                return false;
            }

            m_bufferIn.push("", 1); // append '\0'

            processJson(m_bufferIn.data(), posCur);

            m_bufferIn.pop(posCur + 1); // also pop '\0'
            m_jsonDepthMax = 0;

            return true; // no more data needed
        }
    }
    catch (std::exception& e)
    {
        m_cerr << "Failed to process the response: " << e.what() << "\n";
        return true;
    }

    // more data needed
    return false;
}

void Client::processJson(const char* data, size_t size)
{
    m_cout << std::string_view(data, size) << "\n";
}

void Client::reset() noexcept
{
    m_jsonDepth = 0;
    m_jsonDepthMax = 0;
    m_bufferIn.reset();
}

} // namespace Kesctl {}
