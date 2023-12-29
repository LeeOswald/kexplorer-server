#pragma once

#include <kesrv/propertybag.hxx>

namespace Kes
{

namespace Util
{

namespace Request
{

KESRV_EXPORT std::string simple(const char* command);

namespace Props
{

using Command = PropertyInfo<std::string, KES_PROPID("request.request"), "Request", PropertyFormatter<std::string>>;

} // namespace Props {}

namespace Private
{

void registerAll();

} // namespace Private {}

} // namespace Request {}


namespace Response
{

KESRV_EXPORT std::string fail(const char* reason);

constexpr const char* const Success = "success";
constexpr const char* const Fail = "fail";

namespace Props
{

using Status = PropertyInfo<std::string, KES_PROPID("response.status"), "Status", PropertyFormatter<std::string>>;
using Reason = PropertyInfo<std::string, KES_PROPID("response.reason"), "Reason", PropertyFormatter<std::string>>;
using Version = PropertyInfo<std::string, KES_PROPID("response.srv_version"), "Server Version", PropertyFormatter<std::string>>;

} // namespace Props {}

namespace Private
{

void registerAll();

} // namespace Private {}

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
