PROJECT(FF_BENCHMARK)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)
#add_definitions(-g)
add_definitions(-O2)
add_definitions(-DNDEBUG)
add_definitions(-DUSING_LOCK_FREE_QUEUE)
# SET(DEBUG_DEFS "")

SET(LU_LIST main.cpp)
		
include_directories(${PROJECT_SOURCE_DIR}/../../../ff/include)		
SET(LIBS_LINK_DIRS ${PROJECT_SOURCE_DIR}/../../../build)
LINK_DIRECTORIES(${LIBS_LINK_DIRS})


ADD_EXECUTABLE(lu ${LU_LIST})
TARGET_LINK_LIBRARIES(lu ff pthread)
#TARGET_LINK_LIBRARIES(quicksort ff pthread)
#TARGET_LINK_LIBRARIES(random ff pthread)
#TARGET_LINK_LIBRARIES(canny ${wxWidgets_LIBRARIES} ff pthread)
