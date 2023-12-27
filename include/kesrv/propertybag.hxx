#pragma once

#include <kesrv/json.hxx>
#include <kesrv/log.hxx>
#include <kesrv/property.hxx>

#include <unordered_map>
#include <variant>
#include <vector>

namespace Kes
{

class PropertyBag
{
public:
    using Array = std::vector<std::unique_ptr<PropertyBag>>;
    using Table = std::unordered_map<std::string, std::unique_ptr<PropertyBag>>;

    explicit PropertyBag(Property&& prop) noexcept
        : m_var(std::move(prop))
    {}

    explicit PropertyBag(Array&& a) noexcept
        : m_var(std::move(a))
    {}

    explicit PropertyBag(Table&& t) noexcept
        : m_var(std::move(t))
    {}

    PropertyBag(const PropertyBag&) = delete;
    PropertyBag(PropertyBag&&) noexcept = default;
    PropertyBag& operator=(const PropertyBag&) = delete;
    PropertyBag& operator=(PropertyBag&&) = default;

    constexpr bool isProperty() const noexcept
    {
        return m_var.index() == 0;
    }

    constexpr bool isArray() const noexcept
    {
        return m_var.index() == 1;
    }

    constexpr bool isTable() const noexcept
    {
        return m_var.index() == 2;
    }

    constexpr const Property& property() const noexcept
    {
        return std::get<0>(m_var);
    }

    constexpr Property& property() noexcept
    {
        return std::get<0>(m_var);
    }

    constexpr const Array& array() const noexcept
    {
        return std::get<1>(m_var);
    }

    constexpr Array& array() noexcept
    {
        return std::get<1>(m_var);
    }

    constexpr const Table& table() const noexcept
    {
        return std::get<2>(m_var);
    }

    constexpr Table& table() noexcept
    {
        return std::get<2>(m_var);
    }

private:
    std::variant<Property, Array, Table> m_var;
};


Property propertyFromJson(const char* name, const Json::Value& v, Log::ILog* log = nullptr);


} // namespace Kes {}