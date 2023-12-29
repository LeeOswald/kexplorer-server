#include <kesrv/knownprops.hxx>
#include <kesrv/processmanager/processprops.hxx>


namespace Kes
{

namespace ProcessProps
{

namespace Private
{

KESRV_EXPORT void registerAll()
{
    registerProperty(new PropertyInfoWrapper<ProcessProps::Process>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::DeletedProcess>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::ProcessList>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::DeletedProcessList>);

    registerProperty(new PropertyInfoWrapper<ProcessProps::Error>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::Pid>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::PPid>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::PGrp>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::Tpgid>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::Session>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::StatComm>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::Comm>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::CmdLine>);
    registerProperty(new PropertyInfoWrapper<ProcessProps::Exe>);

}


} // namespace Private {}

} // namespace ProcessProps {}

} // namespace Kes {}