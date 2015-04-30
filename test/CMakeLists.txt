PROJECT(FF_TEST)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)
enable_testing()

#add_definitions(-DUSING_LOCK_FREE_QUEUE)
#add_definitions(-DUSING_FF_SPIN_STEAL_QUEUE)
#add_definitions(-DUSING_MIMO_QUEUE)
add_definitions(-DUSING_WORK_STEALING_QUEUE)


if(Release)
	message(STATUS "This is a Release version!")
	add_definitions(-O2)
	add_definitions(-DNDEBUG)
else(Release)
    add_definitions(-g)
    #add_definitions(-DFUNCTION_FLOW_DEBUG)
    include_directories(${PROJECT_SOURCE_DIR}/../libs)
endif(Release)

include_directories(${PROJECT_SOURCE_DIR}/../ff/include)


find_package(Boost)
if(Boost_FOUND)
	message(STATUS "found boost: ${Boost_LIBRARY_DIRS}")
	include_directories(${Boost_INCLUDE_DIR})
	link_directories(${Boost_LIBRARY})
	SET(LIBS_LINK_LIBS boost_test_exec_monitor ff pthread)
else (Boost_FOUND)
	message(STATUS "can't find boost")
	SET(LIBS_LINK_LIBS ff pthread)
endif (Boost_FOUND)
link_directories(${PROJECT_SOURCE_DIR}/../bin)


ADD_EXECUTABLE(test_interface test_interface.cpp)
TARGET_LINK_LIBRARIES(test_interface ${LIBS_LINK_LIBS})
add_test(test_interface test_interface)

ADD_EXECUTABLE(test_fib test_fib.cpp)
TARGET_LINK_LIBRARIES(test_fib ${LIBS_LINK_LIBS})
add_test(test_fib test_fib)

ADD_EXECUTABLE(test_callable test_callable.cpp)
TARGET_LINK_LIBRARIES(test_callable ${LIBS_LINK_LIBS})
add_test(test_callable test_callable)

add_executable(test_func_with_one_type test_func_with_type.cpp)
target_link_libraries(test_func_with_one_type ${LIBS_LINK_LIBS})
add_test(test_func_with_one_type test_func_with_one_type)

add_executable(test_compatible test_compatible.cpp)
target_link_libraries(test_compatible ${LIBS_LINK_LIBS})
add_test(test_compatible test_compatible)

ADD_EXECUTABLE(test_then test_then.cpp)
TARGET_LINK_LIBRARIES(test_then ${LIBS_LINK_LIBS})
add_test(test_then test_then)

ADD_EXECUTABLE(test_paragroup test_paragroup.cpp)
TARGET_LINK_LIBRARIES(test_paragroup ${LIBS_LINK_LIBS})
add_test(test_paragroup test_paragroup)

add_executable(test_bool_expr test_bool_expr.cpp)
target_link_libraries(test_bool_expr ${LIBS_LINK_LIBS})
add_test(test_bool_expr_test test_bool_expr)

ADD_EXECUTABLE(test_any test_any.cpp)
TARGET_LINK_LIBRARIES(test_any ${LIBS_LINK_LIBS})
add_test(test_any test_any)

add_executable(test_miso_queue test_miso_queue.cpp)
target_link_libraries(test_miso_queue ${LIBS_LINK_LIBS})
add_test(test_miso_queue test_miso_queue)

add_executable(test_paracontainer test_paracontainer.cpp)
target_link_libraries(test_paracontainer ${LIBS_LINK_LIBS})
add_test(test_paracontainer test_paracontainer)

add_executable(test_simo_queue test_simo_queue.cpp)
target_link_libraries(test_simo_queue ${LIBS_LINK_LIBS})
add_test(test_simo_queue test_simo_queue)

add_executable(test_gtwsq test_gtwsq.cpp)
target_link_libraries(test_gtwsq pthread)
add_test(test_gtwsq test_gtwsq)


