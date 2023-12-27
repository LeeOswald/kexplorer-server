#include <kesrv/exception.hxx>
#include <kesrv/processmanager/processprops.hxx>

namespace Kes
{

KESRV_EXPORT void initialize()
{
    Kes::ExceptionProps::Private::registerAll();
    Kes::ProcessProps::Private::registerAll();
}

KESRV_EXPORT void finalize()
{

}

} // namespace Kes {}