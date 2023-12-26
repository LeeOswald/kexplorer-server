#pragma once

#include <kesrv/util/continuousbuffer.hxx>
#include <kesrv/util/readbuffer.hxx>

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

    void command(const std::string& cmd);
    void stop() noexcept;

private:
    bool checkConnection() noexcept;
    void write(std::shared_ptr<std::string> data) noexcept;
    void onWrite(const boost::system::error_code& ec, size_t transferred, std::shared_ptr<std::string> data) noexcept;
    void read(Kes::Util::ReadBuffer::Ptr buffer) noexcept;
    void onRead(const boost::system::error_code& ec, size_t transferred, Kes::Util::ReadBuffer::Ptr buffer) noexcept;
    bool process(const char* data, size_t size) noexcept;
    void processJson(const char* data, size_t size);
    void reset() noexcept;

    const size_t kBufferSize = 65536;
    const size_t kBufferLimit = 64 * 1024 * 1024;

    boost::asio::io_context& m_io;
    bool m_verbose;
    std::ostream& m_cout;
    std::ostream& m_cerr;
    bool m_stop = false;
    std::vector<boost::asio::ip::tcp::endpoint> m_endpoints;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
    Kes::Util::ContinuousBuffer m_bufferIn;
    size_t m_jsonDepth = 0;
    size_t m_jsonDepthMax = 0;
};


} // namespace Kesctl {}
