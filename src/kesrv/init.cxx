#include <kesrv/exception.hxx>

#if KES_LINUX
    #include <kesrv/processmanager/processprops.hxx>
#endif

namespace Kes
{

KESRV_EXPORT void initialize()
{
    Kes::ExceptionProps::Private::registerAll();

#if KES_LINUX
    Kes::ProcessProps::Private::registerAll();
#endif
}

KESRV_EXPORT void finalize()
{

}

} // namespace Kes {}
