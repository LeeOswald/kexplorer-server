#pragma once

#include <kesrv/propertybag.hxx>
#include <kesrv/request.hxx>

namespace Kes
{

namespace Util
{

namespace Request
{

KESRV_EXPORT std::string simple(Kes::Request::Id id, const char* command);

namespace Props
{

namespace Private
{

void registerAll();

} // namespace Private {}

} // namespace Props {}

} // namespace Request {}


namespace Response
{

KESRV_EXPORT std::string fail(Kes::Request::Id id, const char* reason);

namespace Props
{

namespace Private
{

void registerAll();

} // namespace Private {}

} // namespace Props {}

} // namespace Response {}


template <typename PropertyInfoT, typename T>
void addToTable(PropertyBag& table, T&& val)
{
    assert(table.isTable());

    table.table().insert(
        { 
            PropertyInfoT::idstr(), 
            std::make_unique<PropertyBag>(
                PropertyInfoT::idstr(), 
                Property(PropertyInfoT::id(), std::forward<T>(val))
            ) 
        }
    );
}

template <typename PropertyInfoT>
void addToTable(PropertyBag& table, PropertyBag&& val)
{
    assert(table.isTable());

    table.table().insert(
        { 
            PropertyInfoT::idstr(), 
            std::make_unique<PropertyBag>(std::move(val))
        }
    );
}

template <typename PropertyInfoT, typename T>
void addToArray(PropertyBag& array, T&& val)
{
    assert(array.isArray());

    array.array().push_back(
        std::make_unique<PropertyBag>(
            std::string(), 
            Property(PropertyInfoT::id(), std::forward<T>(val))
        ) 
    );
}

template <typename PropertyInfoT>
void addToArray(PropertyBag& array, PropertyBag&& val)
{
    assert(array.isArray());

    array.array().push_back(std::make_unique<PropertyBag>(std::move(val)));
}

} // namespace Util {}

} // namespace Kes {}
