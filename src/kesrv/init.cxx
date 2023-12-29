#include <kesrv/exception.hxx>
#include <kesrv/util/request.hxx>

#if KES_LINUX
    #include <kesrv/processmanager/processprops.hxx>
#endif

namespace Kes
{

KESRV_EXPORT void initialize()
{
    Kes::ExceptionProps::Private::registerAll();
    Kes::Util::Request::Private::registerAll();
    Kes::Util::Response::Private::registerAll();

#if KES_LINUX
    Kes::ProcessProps::Private::registerAll();
#endif
}

KESRV_EXPORT void finalize()
{

}

} // namespace Kes {}
