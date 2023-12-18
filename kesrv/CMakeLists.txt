add_library(${KES_SRVLIB} SHARED
    kesrv.hxx
    procfs/procfs.cxx
    procfs/procfs.hxx
)


add_compile_definitions(KES_KESRV_EXPORTS=1)

target_compile_features(${KES_SRVLIB} PUBLIC ${KES_CXX_FEATURES})
