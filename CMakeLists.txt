PROJECT(FunctionFlow)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)
add_definitions(-g)
SET(DEBUG_DEFS "")

SET(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/ff/include)

SET(LIBS_LINK_DIRS "/home/athrun/boost_1_46_1/stage/lib")
SET(LIBS_LINK_LIBS boost_date_time boost_thread log4cplus)

add_subdirectory(libs)
add_subdirectory(test)
add_subdirectory(benchmark)
add_subdirectory(ff/runtime)
add_subdirectory(ff/include/runtime)