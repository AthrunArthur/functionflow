PROJECT(FF_TEST)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
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

link_directories(${PROJECT_SOURCE_DIR}/../bin)
include_directories(${PROJECT_SOURCE_DIR}/../ff/include)
include_directories(${PROJECT_SOURCE_DIR}/gtest/googletest/include)
link_directories(${PROJECT_BINARY_DIR}/gtest/googletest)


add_subdirectory(gtest/googletest)

set(link_libs ff gtest)

set(src_list main.cpp
  test_any.cpp
  test_bool_expr.cpp
  test_callable.cpp
  test_compatible.cpp
  test_fib.cpp
  test_func_with_type.cpp
  test_gtwsq.cpp
  test_interface.cpp
  test_paracontainer.cpp
  test_paragroup.cpp
  test_then.cpp)

add_executable(ff_test ${src_list})
target_link_libraries(ff_test ${link_libs})

#add_dependencies(test gtest)
