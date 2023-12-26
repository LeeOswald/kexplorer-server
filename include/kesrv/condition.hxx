#pragma once

#include <kesrv/kesrv.hxx>

#include <condition_variable>
#include <mutex>

namespace Kes
{


//
// stateful condition variable
//

class KESRV_EXPORT Condition final
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

    Condition(const Condition&) = delete;
    Condition& operator=(const Condition&) = delete;

    Condition(Condition&&) = delete;
    Condition& operator=(Condition&&) = delete;

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

        if (m_autoReset)
            m_signaled = false;
    }

    bool wait(std::chrono::milliseconds duration) noexcept
    {
        std::unique_lock l(m_mutex);

        auto success = m_cv.wait_for(l, duration, [this]() { return m_signaled; });

        if (m_autoReset)
            m_signaled = false;

        return success;
    }

private:
    std::condition_variable m_cv;
    std::mutex m_mutex;
    bool m_signaled;
    bool m_autoReset;
};



} // namespace Kes {}
