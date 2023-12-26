#include <kesrv/knownprops.hxx>


namespace Kes
{

KESRV_EXPORT void initialize()
{
    Kes::ExceptionProps::Private::registerAll();
}

KESRV_EXPORT void finalize()
{

}

} // namespace Kes {}