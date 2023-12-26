#include <kesrv/knownprops.hxx>

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


KESRV_EXPORT void registerProperty(IPropertyInfo* pi)
{
    auto r = registry();
    std::lock_guard l(r->mutex);

    [[maybe_unused]] auto ret = r->props.insert({ pi->id(), pi });
    assert(ret.second); // check if the property has not been registered yet
}

KESRV_EXPORT IPropertyInfo* lookupProperty(PropId id)
{
    auto r = registry();
    std::lock_guard l(r->mutex);

    auto it = r->props.find(id);
    if (it == r->props.end())
        return nullptr;

    return it->second;
}


namespace Private
{

KESRV_EXPORT void registerAll()
{
    registerProperty(new PropertyInfoWrapper<DecodedError>);
    registerProperty(new PropertyInfoWrapper<PosixErrorCode>);
}

} // namespace Private {}

} // namespace ExceptionProps {}

} // namespace Kes {}