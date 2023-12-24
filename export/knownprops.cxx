#include <export/knownprops.hxx>

#include <mutex>
#include <unordered_map>

namespace Kes
{

namespace ExceptionProps
{

namespace
{

struct Registry
{
    std::mutex mutex;
    std::unordered_map<PropId, IPropertyInfo*> props;
};

Registry* registry()
{
    // this will not ever be deleted because exception handlers
    // may lookup properties during program termination
    static Registry* s_registry = new Registry;
    return s_registry;
}

} // namespace {}


KESCOMMON_EXPORT void registerProperty(IPropertyInfo* pi)
{
    auto r = registry();
    std::lock_guard l(r->mutex);

    [[maybe_unused]] auto ret = r->props.insert({ pi->id(), pi });
    assert(ret.second); // check if the property has not been registered yet
}

KESCOMMON_EXPORT IPropertyInfo* lookupProperty(PropId id)
{
    auto r = registry();
    std::lock_guard l(r->mutex);

    auto it = r->props.find(id);
    if (it == r->props.end())
        return nullptr;

    return it->second;
}

KESCOMMON_EXPORT void registerAll()
{
    registerProperty(new PropertyInfoWrapper<DecodedError>);
    registerProperty(new PropertyInfoWrapper<PosixErrorCode>);
}


} // ExceptionProps {}

} // namespace Kes {}