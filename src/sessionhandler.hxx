#pragma once

#include <export/log.hxx>

#include <vector>


namespace Kes
{

namespace Private
{


struct SessionHandlerOptions
{
    size_t inBufferSize;
    Kes::Log::ILog* log;

    explicit SessionHandlerOptions(size_t inBufferSize, Kes::Log::ILog* log)
        : inBufferSize(inBufferSize)
        , log(log)
    {}
};


class SessionHandler final
{
public:
    ~SessionHandler();
    explicit SessionHandler(const SessionHandlerOptions& options, const std::string& peerAddr);

    SessionHandler(const SessionHandler&) = delete;
    SessionHandler& operator=(const SessionHandler&) = delete;

    SessionHandler(SessionHandler&&) = delete;
    SessionHandler& operator=(SessionHandler&&) = delete;

    void close() noexcept;
    bool process(const char* data, size_t size) noexcept;
    const std::string& peer() const noexcept { return m_peerAddr; }

private:
    class ContinuousBuffer final
    {
    public:
        explicit ContinuousBuffer(size_t size)
            : m_buffer(size)
        {}

        ContinuousBuffer(const ContinuousBuffer&) = delete;
        ContinuousBuffer& operator=(const ContinuousBuffer&) = delete;

        ContinuousBuffer(ContinuousBuffer&&) = delete;
        ContinuousBuffer& operator=(ContinuousBuffer&&) = delete;

        size_t capacity() const noexcept
        {
            return m_buffer.size();
        }

        size_t space() const noexcept
        {
            return m_buffer.size() - (m_start + m_used);
        }

        size_t used() const noexcept
        {
            return m_used;
        }

        const char* data() const noexcept
        {
            if (!m_used)
                return nullptr;
            else
                return m_buffer.data() + m_start;
        }

        char* data() noexcept
        {
            if (!m_used)
                return nullptr;
            else
                return m_buffer.data() + m_start;
        }

        void push(const char* data, size_t size)
        {
            // have we got enough room?
            if (m_start + m_used + size > m_buffer.size())
            {
                if (m_start > 0)
                {
                    // maybe we can shift the existing data
                    std::memmove(m_buffer.data(), m_buffer.data() + m_start, m_used);
                    m_start = 0;
                }

                if (m_start + m_used + size > m_buffer.size())
                {
                    // we have to enlarge the buffer
                    m_buffer.resize(m_start + m_used + size);
                }
            }

            std::memcpy(m_buffer.data() + m_start, data, size);
            m_used += size;
        }

        size_t pop(size_t size) noexcept
        {
            if (size > m_used)
                size = m_used;

            m_start += size;
            m_used -= size;

            return size;
        }

    private:
        std::vector<char> m_buffer;
        size_t m_start = 0;
        size_t m_used = 0;
    };

    SessionHandlerOptions m_options;
    std::string m_peerAddr;
    ContinuousBuffer m_inBuffer;
};



} // namespace Private {}

} // namespace Kes {}
