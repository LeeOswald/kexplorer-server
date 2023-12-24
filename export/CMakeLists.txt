if(KES_LINUX EQUAL 1)
    set(PLATFORM_FILES
        callstack_posix.cxx
        util/posixerror.cxx
        util/posixerror.hxx
    )
endif()


add_library(${KES_COMMON} SHARED
    callstack.hxx
    condition.hxx
    crc32.hxx
    exception.cxx
    exception.hxx
    fixedstring.hxx
    json.hxx
    knownprops.cxx
    knownprops.hxx
    log.hxx
    platform.hxx
    property.hxx
    sourcelocation.hxx
    stringliteral.hxx
    util/autoptr.hxx
    util/continuousbuffer.hxx
    util/exceptionutil.cxx
    util/exceptionutil.hxx
    util/format.cxx
    util/format.hxx
    util/netutil.cxx
    util/netutil.hxx
    util/readbuffer.hxx
    util/request.cxx
    util/request.hxx
    ${PLATFORM_FILES}
)


add_compile_definitions(KES_COMMON_EXPORTS=1)

target_compile_features(${KES_COMMON} PUBLIC ${KES_CXX_FEATURES})
