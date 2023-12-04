set(TARGET kexplorer-ctl)

# require Boost
find_package(Boost REQUIRED COMPONENTS system program_options)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif()

set(BOOST_LIBRARIES Boost::program_options)


add_executable(${TARGET} 
    client.cxx
    client.hxx
    commands.hxx
    main.cxx
)


target_link_libraries(${TARGET} PRIVATE ${PLATFORM_LIBRARIES} ${BOOST_LIBRARIES})

target_compile_features(${TARGET} PUBLIC ${KES_CXX_FEATURES})
