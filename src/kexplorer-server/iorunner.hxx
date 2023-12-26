#pragma once

#include <kesrv/log.hxx>

#include <thread>
#include <vector>

#include <boost/asio.hpp>

namespace Kes
{

namespace Private
{


class IoRunner final
{
public:
    ~IoRunner()
    {
        m_io.stop();

        for (auto& worker: m_workers)
        {
            if (worker->joinable())
            {
                worker->join();
            }
        }
    }

    explicit IoRunner(size_t threadCount, Kes::Log::ILog* log)
        : m_log(log)
        , m_io()
        , m_wg(m_io.get_executor())
    {
        m_log->write(Kes::Log::Level::Debug, "IoRunner: starting");

        m_workers.reserve(threadCount);
        for (auto i = 0; i < threadCount; ++i)
        {
            m_workers.push_back(std::make_unique<std::thread>([this, i]() { run(i); }));
        }
    }

    IoRunner(const IoRunner&) = delete;
    IoRunner& operator=(const IoRunner&) = delete;

    IoRunner(IoRunner&&) = delete;
    IoRunner& operator=(IoRunner&&) = delete;

    boost::asio::io_context& io_context() noexcept
    {
        return m_io;
    }

private:
    void run(size_t index) noexcept
    {
        m_log->write(Kes::Log::Level::Debug, "IoRunner: thread %d started", index);

        try
        {
            m_io.run();
        }
        catch (std::exception& e)
        {
            m_log->write(Kes::Log::Level::Fatal, "IoRunner: thread %d crashed: %s", index, e.what());
        }

        m_log->write(Kes::Log::Level::Debug, "IoRunner: thread %d stopped", index);
    }

    Kes::Log::ILog* m_log;
    boost::asio::io_context m_io;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_wg;
    std::vector<std::unique_ptr<std::thread>> m_workers;
};


} // namespace Private {}

} // namespace Kes {}
