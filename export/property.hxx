#pragma once

#include <export/crc32.hxx>
#include <export/stringliteral.hxx>

#include <any>
#include <array>


namespace Kes
{

template <typename ValueTypeT, uint32_t IdV = 0, typename UnderlyingTypeT = ValueTypeT>
struct PropertyTypeTraits
{
    using Id = std::integral_constant<uint32_t, IdV>;
    using ValueType = ValueTypeT;
    using UnderlyingType = UnderlyingTypeT;
};


template <typename ValueTypeOrTraitsT, StringLiteral Name, typename ValueTypeTraitsT = PropertyTypeTraits<ValueTypeOrTraitsT>>
class PropertyInfo
{
private:
    template <typename T>
    struct ChooseValueType
    {
        using Type = T;
    };

    template <typename ValueTypeT, uint32_t IdV, typename UnderlyingTypeT>
    struct ChooseValueType<PropertyTypeTraits<ValueTypeT, IdV, UnderlyingTypeT>>
    {
        using Type = ValueTypeT;
    };

public:
    using ValueType = typename ChooseValueType<ValueTypeOrTraitsT>::Type;
    using ValueTypeTraits = std::conditional<std::is_same<ValueType, ValueTypeOrTraitsT>::value, ValueTypeTraitsT, ValueTypeOrTraitsT>;

    template <typename T>
    explicit constexpr PropertyInfo(T&& value) noexcept
        : m_value(std::forward<T&&>(value))
    {}

    constexpr const ValueType& value() const noexcept
    {
        return m_value;
    }

    static constexpr const char* name() noexcept
    {
        return fromStringLiteral<Name>();
    }

private:
    ValueType m_value;
};


using PropId = uint32_t;

using PropName = std::array<char, 64>;


} // namespace Kes {}


#define KES_PROPID(s)   Kes::crc32(s)

