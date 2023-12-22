#pragma once

#include <export/crc32.hxx>
#include <export/property.hxx>
#include <export/sourcelocation.hxx>


namespace Kes
{


class KESCOMMON_EXPORT Exception
    : public std::exception
{
public:
    Exception() = default;

    template <typename MessageT>
    explicit Exception(SourceLocation source, MessageT&& message)
        : m_source(source)
        , m_context(std::make_shared<Context>(std::forward<MessageT>(message)))
    {}

    template <typename MessageT, typename PropT, typename... ExceptionProps>
    explicit Exception(SourceLocation source, MessageT&& message, PropT&& prop, ExceptionProps&&... props)
        : m_source(source)
        , m_context(std::make_shared<Context>(std::forward<MessageT>(message), std::forward<PropT>(prop), std::forward<ExceptionProps>(props)...))
    {}

    const char* what() const noexcept override
    {
        if (m_context)
            return m_context->message.c_str();
        return "Unknown exception";
    }

    const SourceLocation& source() const noexcept
    {
        return m_source;
    }

    const std::string* message() const noexcept
    {
        if (m_context)
            return &m_context->message;
        return nullptr;
    }

    const PropertyBag* properties() const noexcept
    {
        if (m_context)
            return &m_context->properties;
        return nullptr;
    }

    const Property* find(PropId id) const noexcept
    {
        if (!m_context)
            return nullptr;

        for (auto& prop: m_context->properties)
        {
            if (prop.id == id)
                return &prop;
        }

        return nullptr;
    }

    template <typename ValueT>
    Exception& add(const std::string_view& name, PropId id, ValueT&& value)
    {
        if (m_context)
        {
            m_context->addProp(name, id, std::forward<ValueT&&>(value));
        }

        return *this;
    }

    template <typename PropT>
    Exception& add(PropT&& prop)
    {
        if (m_context)
        {
            m_context->addProp(prop.name(), prop.id(), (std::forward<PropT>(prop)).value());
        }

        return *this;
    }

private:
    struct Context
    {
        std::string message;
        PropertyBag properties;

        template <typename MessageT>
        Context(MessageT&& message)
            : message(std::forward<MessageT>(message))
        {}

        template <typename MessageT, typename PropT, typename... ExceptionProps>
        Context(MessageT&& message, PropT&& prop, ExceptionProps&&... props)
            : Context(std::forward<MessageT>(message), std::forward<ExceptionProps>(props)...)
        {
            addProp(std::forward<PropT>(prop));
        }

        template <typename ValueT>
        void addProp(const std::string_view& name, PropId id, ValueT&& value)
        {
            properties.emplace_back(name, id, std::forward<ValueT>(value));
        }

        template <typename PropT>
        void addProp(PropT&& prop)
        {
            properties.emplace_back(prop.name(), prop.id(), (std::forward<PropT>(prop)).value());
        }
    };

    SourceLocation m_source;
    std::shared_ptr<Context> m_context;
};



} // namespace Kes {}



#define KES_HERE() Kes::SourceLocation::current()

