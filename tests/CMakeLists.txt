set(TARGET kestests)

enable_testing()

add_executable(
    ${TARGET}
    common.hpp
    main.cpp
    exception.cpp
    fixedstring.cpp
)

target_link_libraries(${TARGET} gtest_main ${KES_COMMON})

target_compile_features(${TARGET} PUBLIC ${KES_CXX_FEATURES})
