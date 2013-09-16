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

SET(LU_LIST LU/ff/lu_main.cpp)
SET(QS_LIST quick_sort/ff/quick_sort.cpp)
SET(CANNY_LIST canny/ff/run_canny.cpp
		canny/ff/canny_edge_detector.cpp)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include( "${wxWidgets_USE_FILE}")		
		
include_directories(${PROJECT_SOURCE_DIR}/../ff/include)		
SET(LIBS_LINK_DIRS ${PROJECT_SOURCE_DIR}/../build)
LINK_DIRECTORIES(${LIBS_LINK_DIRS})

# ADD_DEFINITIONS(${DEBUG_DEFS})

ADD_EXECUTABLE(lu ${LU_LIST})
ADD_EXECUTABLE(quicksort ${QS_LIST})
ADD_EXECUTABLE(canny ${CANNY_LIST})

TARGET_LINK_LIBRARIES(lu ff pthread)
TARGET_LINK_LIBRARIES(quicksort ff pthread)
TARGET_LINK_LIBRARIES(canny ${wxWidgets_LIBRARIES} ff pthread)