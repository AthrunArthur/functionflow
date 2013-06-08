PROJECT(FunctionFlow)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})

SET(DEBUG_DEFS "")

SET(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/ff/include "/home/athrun/boost_1_46_1/stage/include")

SET(LIBS_LINK_DIRS "/home/athrun/boost_1_46_1/stage/lib")
SET(LIBS_LINK_LIBS boost_date_time boost_thread log4cplus)

add_subdirectory(include)
add_subdirectory(libs)
add_subdirectory(test)
add_subdirectory(benchmark)
add_subdirectory(ff)
add_subdirectory(ff/include)
add_subdirectory(ff/runtime)
add_subdirectory(ff/include/common)
add_subdirectory(ff/include/para)
