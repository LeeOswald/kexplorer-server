#pragma once

#include <kesrv/empty.hxx>
#include <kesrv/property.hxx>
#include <kesrv/sourcelocation.hxx>

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

    PropertyBag() noexcept
        : m_name()
        , m_var(Empty())
    {}

    template <typename NameT>
    explicit PropertyBag(NameT&& name, Property&& prop)
        : m_name(std::forward<NameT>(name))
        , m_var(std::move(prop))
    {}

    template <typename NameT>
    explicit PropertyBag(NameT&& name, Array&& a)
        : m_name(std::forward<NameT>(name))
        , m_var(std::move(a))
    {}

    template <typename NameT>
    explicit PropertyBag(NameT&& name, Table&& t)
        : m_name(std::forward<NameT>(name))
        , m_var(std::move(t))
    {}

    PropertyBag(const PropertyBag&) = delete;
    PropertyBag(PropertyBag&&) noexcept = default;
    PropertyBag& operator=(const PropertyBag&) = delete;
    PropertyBag& operator=(PropertyBag&&) = default;

    constexpr bool isEmpty() const noexcept
    {
        return m_var.index() == 0;
    }

    constexpr bool isProperty() const noexcept
    {
        return m_var.index() == 1;
    }

    constexpr bool isArray() const noexcept
    {
        return m_var.index() == 2;
    }

    constexpr bool isTable() const noexcept
    {
        return m_var.index() == 3;
    }

    constexpr const Property& property() const noexcept
    {
        return std::get<1>(m_var);
    }

    constexpr Property& property() noexcept
    {
        return std::get<1>(m_var);
    }

    constexpr const Array& array() const noexcept
    {
        return std::get<2>(m_var);
    }

    constexpr Array& array() noexcept
    {
        return std::get<2>(m_var);
    }

    constexpr const Table& table() const noexcept
    {
        return std::get<3>(m_var);
    }

    constexpr Table& table() noexcept
    {
        return std::get<3>(m_var);
    }

    constexpr const std::string& name() const noexcept
    {
        return m_name;
    }

private:
    std::string m_name;
    std::variant<Empty, Property, Array, Table> m_var;
};


struct IPropertyErrorHandler
{
    virtual CallbackResult handle(SourceLocation where, const std::string& message) noexcept = 0;

protected:
    virtual ~IPropertyErrorHandler() {}
};


// NOTE: this will trash the input buffer
KESRV_EXPORT PropertyBag propertyBagFromJson(char* json, IPropertyErrorHandler* eh);

KESRV_EXPORT std::string propertyBagToJson(const PropertyBag& bag);


} // namespace Kes {}
