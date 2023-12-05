set(TARGET kexplorer-server)

# require pthreads
find_package(Threads REQUIRED)
set(PLATFORM_LIBRARIES Threads::Threads)

# require Boost
find_package(Boost REQUIRED COMPONENTS system program_options)

if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  link_directories(${Boost_LIBRARY_DIRS})
endif()

set(BOOST_LIBRARIES Boost::program_options)


add_executable(${TARGET} 
    iorunner.hxx
    logger.cxx
    logger.hxx
    main.cxx
    sessionhandler.hxx
    sessionhandler.cxx
    tcpserver.hxx
)


target_link_libraries(${TARGET} PRIVATE ${PLATFORM_LIBRARIES} ${BOOST_LIBRARIES} kescommon)

target_compile_features(${TARGET} PUBLIC ${KES_CXX_FEATURES})
