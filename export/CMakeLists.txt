set(TARGET kescommon)

add_library(${TARGET} SHARED
    condition.hxx
    exception.hxx
    json.hxx
    log.hxx
    platform.hxx
    util/continuousbuffer.hxx
    util/netutil.cxx
    util/netutil.hxx
    util/readbuffer.hxx
    util/request.cxx
    util/request.hxx
)


add_compile_definitions(KES_COMMON_EXPORTS=1)

target_compile_features(${TARGET} PUBLIC ${KES_CXX_FEATURES})
