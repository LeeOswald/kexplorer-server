#pragma once

#include <kesrv/log.hxx>
#include <kesrv/util/netutil.hxx>
#include <kesrv/util/readbuffer.hxx>

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
        , m_acceptor(m_io, Kes::Util::endpointFromString(address))
    {
        m_log->write(Kes::Log::Level::Debug, "TcpServer: starting");

        accept();
    }

    TcpServer(const TcpServer&) = delete;
    TcpServer& operator=(const TcpServer&) = delete;

    TcpServer(TcpServer&&) = delete;
    TcpServer& operator=(TcpServer&&) = delete;

    void removeSession(uint32_t id) noexcept
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
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
            Kes::Log::ILog* log
            )
            : m_owner(owner)
            , m_sessionHandlerArgs(sessionHandlerArgs)
            , m_inBufferSize(inBufferSize)
            , m_log(log)
            , m_io(io)
            , m_strand(io)
            , m_socket(socket)
            , m_id(makeNextId())
        {
            m_log->write(Kes::Log::Level::Debug, "TcpServer: session %d created", m_id);
        }

        static Ptr create(
            TcpServer* owner,
            const SessionHandlerArgs& sessionHandlerArgs,
            size_t inBufferSize,
            boost::asio::io_context& io,
            std::shared_ptr<boost::asio::ip::tcp::socket> socket,
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
                    socket,
                    log
                );
            }
            catch (std::exception& e)
            {
                log->write(Kes::Log::Level::Error, "TcpServer: failed to create session: %s", e.what());
                return Ptr();
            }
        }

        void close() noexcept
        {
            if (m_sessionHandler)
            {
                m_sessionHandler->close();
            }

            if (m_socket->is_open())
            {
                boost::system::error_code ec;
                m_socket->shutdown(boost::asio::ip::tcp::socket::shutdown_send);
                m_socket->close();
            }
        }

        std::optional<uint32_t> start() noexcept
        {
            try
            {
                auto peer = m_socket->remote_endpoint();
                auto peerAddr = peer.address().to_string();

                m_sessionHandler.reset(new SessionHandler(m_sessionHandlerArgs, peerAddr, m_id));

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

        uint32_t id() const noexcept
        {
            return m_id;
        }

    private:
        bool read(Kes::Util::ReadBuffer::Ptr buffer) noexcept
        {
            try
            {
                if (!buffer)
                {
                    buffer = Kes::Util::ReadBuffer::create(m_inBufferSize);
                }

                m_socket->async_read_some(
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

        void onRead(const boost::system::error_code& ec, size_t transferred, Kes::Util::ReadBuffer::Ptr buffer) noexcept
        {
            if (ec)
            {
                m_log->write(Kes::Log::Level::Warning, "TcpServer: read() failed: %s", ec.message().c_str());

                close();
                m_owner->removeSession(m_id);
            }
            else
            {
#if KES_DEBUG
                m_log->write(Kes::Log::Level::Debug, "TcpServer: received  %d bytes", transferred);
#endif

                buffer->swap();

                // start another read immediately
                read(buffer);

                // this MUST be noexcept
                auto result = m_sessionHandler->process(buffer->data(buffer->r_index()), transferred);
                if (result.first == CallbackResult::Abort)
                {
                    // abort connection
                    close();
                    m_owner->removeSession(m_id);
                }
                else
                {
                    write(std::make_shared<std::string>(std::move(result.second)));
                }
            }
        }

        void write(std::shared_ptr<std::string> buffer) noexcept
        {
            try
            {
                boost::asio::async_write(
                    *m_socket,
                    boost::asio::const_buffer(buffer->data(), buffer->size()),
                    m_strand.wrap(
                        [this](const boost::system::error_code& ec, size_t transferred)
                        {
                            this->onWrite(ec, transferred);
                        }
                    )
                );
            }
            catch (std::exception& e)
            {
                m_log->write(Kes::Log::Level::Error, "TcpServer: failed to send data: %s", e.what());

                close();
                m_owner->removeSession(m_id);
            }
        }

        void onWrite(const boost::system::error_code& ec, size_t transferred) noexcept
        {
            if (ec)
            {
                m_log->write(Kes::Log::Level::Error, "TcpServer: write() failed: %s", ec.message().c_str());

                close();
                m_owner->removeSession(m_id);
            }
            else
            {
#if KES_DEBUG
                m_log->write(Kes::Log::Level::Debug, "TcpServer: sent  %d bytes", transferred);
#endif
            }
        }

        static uint32_t makeNextId() noexcept
        {
            static std::atomic<uint32_t> nextId = 0;
            return nextId++;
        }

        TcpServer* m_owner;
        std::unique_ptr<SessionHandler> m_sessionHandler;
        const SessionHandlerArgs& m_sessionHandlerArgs;
        size_t m_inBufferSize;
        Kes::Log::ILog* m_log;
        boost::asio::io_context& m_io;
        boost::asio::io_service::strand m_strand;
        std::shared_ptr<boost::asio::ip::tcp::socket> m_socket;
        uint32_t m_id;
    };

    void accept() noexcept
    {
        auto socket = std::make_shared<boost::asio::ip::tcp::socket>(m_io);
        m_acceptor.async_accept(
            *socket,
            m_strand.wrap(
                [this, socket](const boost::system::error_code& ec)
                {
                    onAccept(socket, ec);
                }
            )
        );
    }

    void onAccept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, const boost::system::error_code& ec) noexcept
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

            auto session = Session::create(this, m_sessionHandlerArgs, m_inBufferSize, m_io, socket, m_log);
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
