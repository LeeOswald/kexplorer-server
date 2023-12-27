#include <kesrv/exception.hxx>
#include <kesrv/knownprops.hxx>
#include <kesrv/util/format.hxx>


#include <mutex>
#include <unordered_map>

namespace Kes
{

namespace
{

struct
{
    std::mutex mutex;
    std::unordered_map<PropId, IPropertyInfo*> propsById;
    std::unordered_map<std::string, IPropertyInfo*> propsByName;
} s_registry;


} // namespace {}


KESRV_EXPORT void registerProperty(IPropertyInfo* pi)
{
    std::lock_guard l(s_registry.mutex);

    auto ret1 = s_registry.propsById.insert({ pi->id(), pi });
    if (!ret1.second)
    {
        throw Exception(KES_HERE(), Util::format("Property with ID %08x already registered", pi->id()));
    }

    auto ret2 = s_registry.propsByName.insert({ pi->idstr(), pi });
    if (!ret2.second)
    {
        throw Exception(KES_HERE(), Util::format("Property with ID %s already registered", pi->idstr()));
    }
}

KESRV_EXPORT IPropertyInfo* lookupProperty(PropId id)
{
    std::lock_guard l(s_registry.mutex);

    auto it = s_registry.propsById.find(id);
    if (it == s_registry.propsById.end())
        return nullptr;

    return it->second;
}

KESRV_EXPORT IPropertyInfo* lookupProperty(const char* id)
{
    std::lock_guard l(s_registry.mutex);

    auto it = s_registry.propsByName.find(id);
    if (it == s_registry.propsByName.end())
        return nullptr;

    return it->second;
}

} // namespace Kes {}
