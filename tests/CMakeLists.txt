set(TARGET kestests)

enable_testing()

add_executable(
    ${TARGET}
    common.hpp
    logger.hpp
    logger.cpp
    main.cpp
    exception.cpp
    fixedstring.cpp
    propertybag.cpp
)

target_link_libraries(${TARGET} gtest_main ${KES_SRVLIB})

target_compile_features(${TARGET} PUBLIC ${KES_CXX_FEATURES})
