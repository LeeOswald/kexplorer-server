#pragma once

#include <export/crc32.hxx>
#include <export/fixedstring.hxx>
#include <export/stringliteral.hxx>

#include <any>
#include <vector>


namespace Kes
{

using PropId = uint32_t;

using PropName = FixedString<64>;



template <typename ValueT, PropId PrId, StringLiteral PrName>
class PropertyInfo final
{
public:
    using ValueType = ValueT;
    using Id = std::integral_constant<PropId, PrId>;

    template <typename T>
    explicit constexpr PropertyInfo(T&& value) noexcept
        : m_value(std::forward<T>(value))
    {}

    constexpr PropId id() const noexcept
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

private:
    ValueType m_value;
};


struct Property
{
    Property() = default;

    Property(const std::string_view& name, PropId id, std::any&& value) noexcept
        : name(name)
        , id(id)
        , value(std::move(value))
    {}

    template <typename ValueT>
    Property(const std::string_view& name, PropId id, ValueT&& value) noexcept
        : name(name)
        , id(id)
        , value(std::forward<ValueT>(value))
    {}

    PropName name;
    PropId id;
    std::any value;
};


using PropertyBag = std::vector<Property>;


} // namespace Kes {}


#define KES_PROPID(s)   Kes::crc32(s)

