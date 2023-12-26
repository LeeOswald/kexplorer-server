set(PLATFORM_FILES
    ../../include/kesrv/requestprocessor.hxx
    ../../include/kesrv/processmanager/processmanager.hxx
    ../../include/kesrv/processmanager/procfs.hxx
    ../../include/kesrv/util/posixerror.hxx
    callstack_posix.cxx
    processmgr/processmanager.cxx
    processmgr/procfs.cxx
    util/posixerror_posix.cxx
)

add_library(${KES_SRVLIB} SHARED
    ../../include/kesrv/callstack.hxx
    ../../include/kesrv/condition.hxx
    ../../include/kesrv/exception.hxx
    ../../include/kesrv/fixedstring.hxx
    ../../include/kesrv/json.hxx
    ../../include/kesrv/kesrv.hxx
    ../../include/kesrv/knownprops.hxx
    ../../include/kesrv/log.hxx
    ../../include/kesrv/platform.hxx
    ../../include/kesrv/property.hxx
    ../../include/kesrv/sourcelocation.hxx
    ../../include/kesrv/stringliteral.hxx
    ../../include/kesrv/util/autoptr.hxx
    ../../include/kesrv/util/continuousbuffer.hxx
    ../../include/kesrv/util/crc32.hxx
    ../../include/kesrv/util/exceptionutil.hxx
    ../../include/kesrv/util/format.hxx
    ../../include/kesrv/util/netutil.hxx
    ../../include/kesrv/util/readbuffer.hxx
    ../../include/kesrv/util/request.hxx
    exception.cxx
    init.cxx
    knownprops.cxx
    util/exceptionutil.cxx
    util/format.cxx
    util/netutil.cxx
    util/request.cxx
    ${PLATFORM_FILES}
)


add_compile_definitions(KESRV_EXPORTS=1)

target_compile_features(${KES_SRVLIB} PUBLIC ${KES_CXX_FEATURES})