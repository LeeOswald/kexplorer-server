add_library(${KES_SRVLIB} SHARED
    kesrv.hxx
    processmgr/processmanager.cxx
    processmgr/processmanager.hxx
    processmgr/procfs.cxx
    processmgr/procfs.hxx
)


add_compile_definitions(KES_KESRV_EXPORTS=1)

target_compile_features(${KES_SRVLIB} PUBLIC ${KES_CXX_FEATURES})
