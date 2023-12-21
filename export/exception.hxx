#pragma once

#include <export/crc32.hxx>
#include <export/property.hxx>
#include <export/sourcelocation.hxx>

#if 1
#include <export/knownprops.hxx>
#endif


namespace Kes
{


class Exception
    : public std::exception
{
public:
    Exception() = default;

    template <typename MessageT>
    explicit Exception(MessageT&& message, SourceLocation location = SourceLocation::current())
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
    SourceLocation m_location;
    std::string m_message;
};


} // namespace Kes {}
