#pragma once

#include <export/crc32.hxx>
#include <export/fixedstring.hxx>
#include <export/stringliteral.hxx>

#include <any>
#include <ostream>
#include <vector>


namespace Kes
{

using PropId = uint32_t;

using PropName = FixedString<64>;



template <typename ValueT, PropId PrId, StringLiteral PrName, class FormatterT>
class PropertyInfo final
{
public:
    using ValueType = ValueT;
    using Id = std::integral_constant<PropId, PrId>;
    using Formatter = FormatterT;

    PropertyInfo() noexcept = default;

    template <typename T>
    explicit constexpr PropertyInfo(T&& value) noexcept
        : m_value(std::forward<T>(value))
    {}

    static constexpr PropId id() noexcept
    {
        return Id::value;
    }

    constexpr ValueType&& value() && noexcept
    {
        return std::move(m_value);
    }

    constexpr ValueType& value() & noexcept
    {
        return m_value;
    }

    static constexpr const char* name() noexcept
    {
        return fromStringLiteral<PrName>();
    }

    void format(std::ostream& strem) const
    {
        Formatter f;
        f(m_value, strem);
    }

private:
    ValueType m_value;
};


struct Property
{
    Property() = default;

    Property(PropId id, std::any&& value) noexcept
        : id(id)
        , value(std::move(value))
    {}

    template <typename ValueT>
    Property(PropId id, ValueT&& value) noexcept
        : id(id)
        , value(std::forward<ValueT>(value))
    {}

    PropId id;
    std::any value;
};


template <typename T>
struct PropertyFormatter
{
    using Type = T;
    void operator()(const Property& v, std::ostream& s) { s << std::any_cast<T>(v.value); }
};


struct IPropertyInfo
{
    virtual PropId id() const = 0;
    virtual const char* name() const = 0;
    virtual void format(const Property& v, std::ostream& s) = 0;
};


template <class PropertyInfoT>
struct PropertyInfoWrapper
    : public IPropertyInfo
{
    PropId id() const override
    {
        return PropertyInfoT::id();
    }

    const char* name() const override
    {
        return PropertyInfoT::name();
    }

    void format(const Property& v, std::ostream& s) override
    {
        typename PropertyInfoT::Formatter f;
        f(v, s);
    }
};


} // namespace Kes {}


#define KES_PROPID(s)   Kes::crc32(s)

