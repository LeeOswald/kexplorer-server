#pragma once

#include <export/log.hxx>

#include <atomic>
#include <mutex>
#include <thread>
#include <vector>

#include <boost/asio.hpp>


namespace Kes
{

namespace Private
{


template <class SessionHandlerT, class SessionHandlerArgsT>
class TcpServer final
{
public:
    using SessionHandler = SessionHandlerT;
    using SessionHandlerArgs = SessionHandlerArgsT;

    ~TcpServer()
    {
        m_log->write(Kes::Log::Level::Debug, "TcpServer: shutting down");

        m_stop = true;

        boost::system::error_code ec;
        m_retryTimer.cancel(ec);

        for (auto& session: m_sessions)
        {
            session->close();
        }

        m_acceptor.cancel();
        m_acceptor.close();
    }

    explicit TcpServer(
        boost::asio::io_context& io,
        const SessionHandlerArgs& sessionHandlerArgs,
        const char* address,
        size_t inBufferSize,
        Kes::Log::ILog* log
    )
        : m_sessionHandlerArgs(sessionHandlerArgs)
        , m_inBufferSize(inBufferSize)
        , m_log(log)
        , m_io(io)
        , m_retryTimer(m_io)
        , m_strand(m_io)
        , m_acceptor(m_io, endpointFromString(address))
    {
        m_log->write(Kes::Log::Level::Debug, "TcpServer: starting");

        accept();
    }

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    TcpServer(TcpServer&&) = delete;
    TcpServer& operator=(TcpServer&&) = delete;

    void removeSession(size_t id) noexcept
    {
        std::lock_guard l(m_mutex);

        for (auto it = m_sessions.begin(); it != m_sessions.end(); ++it)
        {
            if (it->get()->id() == id)
            {
                m_sessions.erase(it);
                break;
            }
        }
    }

private:
    class Session final
    {
    public:
        using Ptr = std::shared_ptr<Session>;

        Session(const Session&) = delete;
        Session& operator=(const Session&) = delete;

        Session(Session&&) = delete;
        Session& operator=(Session&&) = delete;

        ~Session()
        {
            m_log->write(Kes::Log::Level::Debug, "TcpServer: session %d destroyed", m_id);
        }

        explicit Session(
            TcpServer* owner,
            const SessionHandlerArgs& sessionHandlerArgs,
            size_t inBufferSize,
            boost::asio::io_context& io,
            Kes::Log::ILog* log
            )
            : m_owner(owner)
            , m_sessionHandlerArgs(sessionHandlerArgs)
            , m_inBufferSize(inBufferSize)
            , m_log(log)
            , m_io(io)
            , m_strand(io)
            , m_socket(io)
            , m_id(makeNextId())
        {
        }

        static Ptr create(
            TcpServer* owner,
            const SessionHandlerArgs& sessionHandlerArgs,
            size_t inBufferSize,
            boost::asio::io_context& io,
            Kes::Log::ILog* log
        ) noexcept
        {
            try
            {
                return std::make_shared<Session>(
                    owner,
                    sessionHandlerArgs,
                    inBufferSize,
                    io,
                    log
                );
            }
            catch (std::exception& e)
            {
                log->write(Kes::Log::Level::Error, "TcpServer: failed to create session: %s", e.what());
                return Ptr();
            }
        }

        boost::asio::ip::tcp::socket& socket() noexcept { return m_socket; }

        void close() noexcept
        {
            if (m_sessionHandler)
            {
                m_sessionHandler->close();
            }

            if (m_socket.is_open())
            {
                boost::system::error_code ec;
                m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_send);
                m_socket.close();
            }
        }

        std::optional<size_t> start() noexcept
        {
            try
            {
                auto peer = m_socket.remote_endpoint();
                auto peerAddr = peer.address().to_string();

                m_sessionHandler.reset(new SessionHandler(m_sessionHandlerArgs, peerAddr));

                read(nullptr);

                m_log->write(Kes::Log::Level::Debug, "TcpServer: session %d started with [%s]", m_id, peerAddr.c_str());
            }
            catch (std::exception& e)
            {
                m_log->write(Kes::Log::Level::Error, "TcpServer: failed to start session: %s", e.what());
                return std::nullopt;
            }

            return m_id;
        }

        size_t id() const noexcept
        {
            return m_id;
        }

    private:
        class ReadBuffer final
        {
        public:
            using Ptr = std::shared_ptr<ReadBuffer>;

            explicit ReadBuffer(size_t size) noexcept
                : m_buffers { std::vector<char>(size), std::vector<char>(size) }
                , m_size(size)
            {
            }

            ReadBuffer(const ReadBuffer&) = delete;
            ReadBuffer& operator=(const ReadBuffer&) = delete;

            ReadBuffer(ReadBuffer&&) = delete;
            ReadBuffer& operator=(ReadBuffer&&) = delete;

            static Ptr create(size_t size)
            {
                return std::make_shared<ReadBuffer>(size);
            }

            constexpr char* data(size_t index) noexcept { return m_buffers[index].data(); }
            constexpr const char* data(size_t index) const noexcept { return m_buffers[index].data(); }
            constexpr size_t size() const noexcept { return m_size; }
            constexpr size_t w_index() const noexcept { return m_active; }
            constexpr size_t r_index() const noexcept { return m_active ^ 1; }
            constexpr void swap() noexcept { m_active ^= 1; }

        private:
            std::vector<char> m_buffers[2];
            size_t m_size;
            size_t m_active = 0;
        };

        bool read(ReadBuffer::Ptr buffer) noexcept
        {
            try
            {
                if (!buffer)
                {
                    buffer = ReadBuffer::create(m_inBufferSize);
                }

                m_socket.async_read_some(
                    boost::asio::buffer(buffer->data(buffer->w_index()), buffer->size()),
                    m_strand.wrap(
                        [this, buffer](const boost::system::error_code& ec, size_t transferred)
                        {
                            this->onRead(ec, transferred, buffer);
                        }
                    )
                );

            }
            catch (std::exception& e)
            {
                m_log->write(Kes::Log::Level::Error, "TcpServer: failed to receive data: %s", e.what());

                close();
                m_owner->removeSession(m_id);

                return false;
            }

            return true;
        }

        void onRead(const boost::system::error_code& ec, size_t transferred, ReadBuffer::Ptr buffer) noexcept
        {
            if (ec)
            {
                m_log->write(Kes::Log::Level::Error, "TcpServer: read() failed: %s", ec.message().c_str());

                close();
                m_owner->removeSession(m_id);
            }
            else
            {
#if 1
                m_log->write(Kes::Log::Level::Debug, "TcpServer: received  %d bytes", transferred);
#endif

                buffer->swap();

                // start another read immediately
                read(buffer);

                // this MUST be noexcept
                auto result = m_sessionHandler->process(buffer->data(buffer->r_index()), transferred);
                if (!result)
                {
                    // abort connection
                    close();
                    m_owner->removeSession(m_id);
                }
            }
        }

        static size_t makeNextId() noexcept
        {
            static std::atomic<size_t> nextId = 0;
            return nextId++;
        }

        TcpServer* m_owner;
        std::unique_ptr<SessionHandler> m_sessionHandler;
        const SessionHandlerArgs& m_sessionHandlerArgs;
        size_t m_inBufferSize;
        Kes::Log::ILog* m_log;
        boost::asio::io_context& m_io;
        boost::asio::io_service::strand m_strand;
        boost::asio::ip::tcp::socket m_socket;
        size_t m_id;
    };

    static boost::asio::ip::tcp::endpoint endpointFromString(const std::string& s)
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

    void accept() noexcept
    {
        auto session = Session::create(this, m_sessionHandlerArgs, m_inBufferSize, m_io, m_log);
        if (!session)
        {
            // retry after 1 second
            m_retryTimer.expires_from_now(boost::posix_time::milliseconds(1000));
            m_retryTimer.async_wait(
                [this](const boost::system::error_code& e)
                {
                    if (!e && !m_stop)
                        accept();
                }
            );

            return;
        }

        m_acceptor.async_accept(
            session->socket(),
            m_strand.wrap(
                [this, session](const boost::system::error_code& ec)
                {
                    onAccept(session, ec);
                }
            )
        );
    }

    void onAccept(Session::Ptr session, const boost::system::error_code& ec) noexcept
    {
        if (m_stop)
            return;

        if (ec)
        {
            m_log->write(Kes::Log::Level::Error, "TcpServer: accept() failed: %s", ec.message().c_str());

            // retry after 1 second
            m_retryTimer.expires_from_now(boost::posix_time::milliseconds(1000));
            m_retryTimer.async_wait(
                [this](const boost::system::error_code& e)
                {
                    if (!e && !m_stop)
                        accept();
                }
            );

        }
        else
        {
            m_log->write(Kes::Log::Level::Debug, "TcpServer: connection accepted");

            // continue accepting clients
            accept();

            {
                std::lock_guard l(m_mutex);
                m_sessions.push_back(session);
            }

            auto id = session->start();
            if (!id.has_value())
            {
                removeSession(*id);
            }
        }
    }

    SessionHandlerArgs m_sessionHandlerArgs;
    size_t m_inBufferSize;
    Kes::Log::ILog* m_log;
    boost::asio::io_context& m_io;
    boost::asio::deadline_timer m_retryTimer;
    boost::asio::io_service::strand m_strand;
    boost::asio::ip::tcp::acceptor m_acceptor;
    bool m_stop = false;
    std::mutex m_mutex;
    std::vector<typename Session::Ptr> m_sessions;
};


} // namespace Private {}

} // namespace Kes {}
