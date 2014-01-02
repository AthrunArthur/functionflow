PROJECT(FunctionFlow)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)

if(Release)
	message(STATUS "This is a Release version!")
	add_definitions(-O2)
	add_definitions(-DNDEBUG)
else(Release)
	message(STATUS "This is a Debug version!")
	add_definitions(-g)
	add_definitions(-DFUNCTION_FLOW_DEBUG)
	include_directories(${PROJECT_SOURCE_DIR}/libs)
	if(NOT EXISTS ${PROJECT_SOURCE_DIR}/libs/ff)
		message(STATUS "require ff, start downloading...")
		file(DOWNLOAD https://github.com/AthrunArthur/ff/archive/master.zip ${PROJECT_SOURCE_DIR}/libs/ff.zip)
		exec_program(unzip ${PROJECT_SOURCE_DIR}/libs ARGS ff.zip */ff/* -d ffcommon)
		exec_program(cp ${PROJECT_SOURCE_DIR}/libs ARGS -r ffcommon/ff-master/ff/ ff/)
		exec_program(rm ${PROJECT_SOURCE_DIR}/libs ARGS -rf ffcommon/)
	endif(NOT EXISTS ${PROJECT_SOURCE_DIR}/libs/ff)
endif(Release)

add_definitions(-DUSING_LOCK_FREE_QUEUE)
#add_definitions(-DUSING_FF_SPIN_STEAL_QUEUE)
#add_definitions(-DUSING_MIMO_QUEUE)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)


include_directories(${PROJECT_SOURCE_DIR}/ff/include)
link_directories(${PROJECT_SOURCE_DIR}/ff/bin)

SET(ff_src ff/runtime/env.cpp 
  ff/runtime/runtime.cpp 
  ff/para/para.cpp 
  ff/common/common.cpp)
ADD_LIBRARY(ff SHARED ${ff_src})
ADD_LIBRARY(ff_s STATIC ${ff_src})
target_link_libraries(ff pthread)
target_link_libraries(ff_s pthread)

SET_TARGET_PROPERTIES(ff_s PROPERTIES OUTPUT_NAME "ff")
 
