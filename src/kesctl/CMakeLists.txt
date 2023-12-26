add_executable(${KES_CTL} 
    client.cxx
    client.hxx
    main.cxx
)


target_link_libraries(${KES_CTL} PRIVATE ${PLATFORM_LIBRARIES} ${BOOST_LIBRARIES} ${KES_SRVLIB})

target_compile_features(${KES_CTL} PUBLIC ${KES_CXX_FEATURES})
