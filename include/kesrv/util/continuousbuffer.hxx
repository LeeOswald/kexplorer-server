#pragma once

#include <kesrv/kesrv.hxx>

#include <vector>


namespace Kes
{

namespace Util
{


class ContinuousBuffer final
{
public:
    explicit ContinuousBuffer(size_t size, size_t limit)
        : m_buffer(size)
        , m_limit(limit)
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

    bool push(const char* data, size_t size)
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
                if (m_start + m_used + size > m_limit)
                    return false;

                m_buffer.resize(m_start + m_used + size);
            }
        }

        std::memcpy(m_buffer.data() + m_start + m_used, data, size);
        m_used += size;

        return true;
    }

    size_t pop(size_t size) noexcept
    {
        if (size > m_used)
            size = m_used;

        m_start += size;
        m_used -= size;

        return size;
    }

    void reset() noexcept
    {
        m_start = 0;
        m_used = 0;
    }

private:
    std::vector<char> m_buffer;
    size_t m_limit;
    size_t m_start = 0;
    size_t m_used = 0;
};



} // namespace Util {}

} // namespace Kes {}
