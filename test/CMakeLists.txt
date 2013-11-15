PROJECT(FF_TEST)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)

#add_definitions(-DUSING_LOCK_FREE_QUEUE)
#add_definitions(-DUSING_FF_SPIN_STEAL_QUEUE)
add_definitions(-DUSING_MIMO_QUEUE)


if(Release)
	message(STATUS "This is a Release version!")
	add_definitions(-O2)
	add_definitions(-DNDEBUG)
else(Release)
    add_definitions(-g)
    add_definitions(-DFUNCTION_FLOW_DEBUG)
    include_directories(${PROJECT_SOURCE_DIR}/../libs)
endif(Release)

include_directories(${PROJECT_SOURCE_DIR}/../ff/include)


#find_package(Boost)
#if(Boost_FOUND)
#	message(STATUS "found boost: ${Boost_LIBRARY_DIRS}")
#	include_directories(${Boost_INCLUDE_DIR})
#	link_directories(${Boost_LIBRARY})
#	SET(LIBS_LINK_LIBS boost_test_exec_monitor ff pthread)
#else (Boost_FOUND)
#	message(STATUS "can't find boost")
#	SET(LIBS_LINK_LIBS ff pthread)
#endif (Boost_FOUND)
SET(LIBS_LINK_LIBS ff pthread)
link_directories(${PROJECT_SOURCE_DIR}/../bin)



###########################
if(Boost_FOUND)
ADD_EXECUTABLE(test test_interface.cpp)
TARGET_LINK_LIBRARIES(test ${LIBS_LINK_LIBS})
endif (Boost_FOUND)

ADD_EXECUTABLE(helloworld hello_world.cpp)
TARGET_LINK_LIBRARIES(helloworld ${LIBS_LINK_LIBS})

ADD_EXECUTABLE(fib fib.cpp)
TARGET_LINK_LIBRARIES(fib ${LIBS_LINK_LIBS})

ADD_EXECUTABLE(test_jmp test_jmp.cpp)
TARGET_LINK_LIBRARIES(test_jmp ${LIBS_LINK_LIBS})

ADD_EXECUTABLE(test_rt test_rt.cpp)
TARGET_LINK_LIBRARIES(test_rt ${LIBS_LINK_LIBS})

ADD_EXECUTABLE(test_then test_then.cpp)
TARGET_LINK_LIBRARIES(test_then ${LIBS_LINK_LIBS})

ADD_EXECUTABLE(test_ws test_work_steal.cpp)
TARGET_LINK_LIBRARIES(test_ws ${LIBS_LINK_LIBS})

ADD_EXECUTABLE(paragroup_test test_paragroup.cpp)
TARGET_LINK_LIBRARIES(paragroup_test ${LIBS_LINK_LIBS})