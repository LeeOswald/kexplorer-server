#pragma once

#include <export/platform.hxx>


#include <condition_variable>
#include <mutex>

namespace Kes
{


class Condition final
{
public:
    ~Condition()
    {
    }

    explicit Condition(bool autoReset, bool initiallySignaled = false)
        : m_signaled(initiallySignaled)
        , m_autoReset(autoReset)
    {
    }

    void set() noexcept
    {
        {
            std::lock_guard l(m_mutex);
            m_signaled = true;
        }

        if (m_autoReset)
            m_cv.notify_one();
        else
            m_cv.notify_all();
    }

    void reset() noexcept
    {
        std::lock_guard l(m_mutex);
        m_signaled = false;
    }

    void wait() noexcept
    {
        std::unique_lock l(m_mutex);

        m_cv.wait(l, [this]() { return m_signaled; });
    }

    bool wait(std::chrono::milliseconds duration) noexcept
    {
        std::unique_lock l(m_mutex);

        return m_cv.wait_for(l, duration, [this]() { return m_signaled; });
    }

private:
    std::condition_variable m_cv;
    std::mutex m_mutex;
    bool m_signaled;
    bool m_autoReset;
};



} // namespace Kes {}
