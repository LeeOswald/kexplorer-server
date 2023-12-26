add_executable(${KES_SERVER}
    globalcmdhandler.cxx
    globalcmdhandler.hxx
    iorunner.hxx
    logger.cxx
    logger.hxx
    main.cxx
    requestprocessor.cxx
    requestprocessor.hxx
    sessionhandler.cxx
    sessionhandler.hxx
    tcpserver.hxx
)


target_link_libraries(${KES_SERVER} PRIVATE ${PLATFORM_LIBRARIES} ${BOOST_LIBRARIES} ${KES_SRVLIB})

target_compile_features(${KES_SERVER} PUBLIC ${KES_CXX_FEATURES})
