#pragma once

#include <export/platform.hxx>


namespace Kes
{


class Exception
    : public std::exception
{
public:
    Exception() = default;

    template <typename MessageT>
    explicit Exception(MessageT&& message)
        : m_message(std::forward<MessageT&&>(message))
    {}

    const char* what() const noexcept override
    {
        return m_message.c_str();
    }

    const std::string& message() const noexcept
    {
        return m_message;
    }

private:
    std::string m_message;
};


} // namespace Kes {}
