PROJECT(FF_TEST)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)
enable_testing()

#add_definitions(-DUSING_LOCK_FREE_QUEUE)
#add_definitions(-DUSING_FF_SPIN_STEAL_QUEUE)
#add_definitions(-DUSING_MIMO_QUEUE)
#add_definitions(-DUSING_MUTEX_QUEUE)
add_definitions(-DUSING_WORK_STEALING_QUEUE)
#add_definitions(-DUSING_FLEXIBLE_QUEUE)

if(Release)
	message(STATUS "This is a Release version!")
	add_definitions(-O2)
	add_definitions(-DNDEBUG)
	message(STATUS "this is release version!")
else(Release)
    add_definitions(-g)
    add_definitions(-O2)
    add_definitions(-DFUNCTION_FLOW_DEBUG)
    include_directories(${PROJECT_SOURCE_DIR}/../libs)
    message(STATUS "this is debug version!")
endif(Release)

if(COUNT_FUNC)
add_definitions(-DFUNC_INVOKE_COUNTER)
endif(COUNT_FUNC)

include_directories(${PROJECT_SOURCE_DIR}/../ff/include)
set(LIBS_LINK_LIBS ff pthread)

link_directories(${PROJECT_SOURCE_DIR}/../bin)


ADD_EXECUTABLE(fftest main.cpp lu.cpp)
TARGET_LINK_LIBRARIES(fftest ${LIBS_LINK_LIBS})
