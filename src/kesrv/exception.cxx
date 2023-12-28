#include <kesrv/exception.hxx>
#include <kesrv/knownprops.hxx>

namespace Kes
{

namespace ExceptionProps
{

namespace Private
{

void registerAll()
{
    registerProperty(new PropertyInfoWrapper<DecodedError>);
    registerProperty(new PropertyInfoWrapper<PosixErrorCode>);
}

} // namespace Private {}

} // namespace ExceptionProps {}

} // namespace Kes {}
