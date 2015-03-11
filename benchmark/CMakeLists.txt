PROJECT(FF_BENCHMARK)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
MESSAGE(STATUS "This is BINARY dir" ${PROJECT_BINARY_DIR})
MESSAGE(STATUS "This is SOURCE dir" ${PROJECT_SOURCE_DIR})
add_definitions(-std=c++11)

#add_definitions(-DUSING_FF_NONBLOCKING_QUEUE)
#add_definitions(-DUSING_LOCK_FREE_QUEUE)
add_definitions(-DUSING_WORK_STEALING_QUEUE)
#add_definitions(-DUSING_MIMO_QUEUE)
#add_definitions(-DUSING_FF_SPIN_STEAL_QUEUE)

if(Debug)
  add_definitions(-g)
  message(STATUS "This is a Debug version!")
  add_definitions(-g)
  add_definitions(-DFUNCTION_FLOW_DEBUG)
  include_directories(${PROJECT_SOURCE_DIR}/../libs)
else(Debug)
  message(STATUS "This is a Release version!")
  add_definitions(-g)
  add_definitions(-O2)
  add_definitions(-DNDEBUG)
endif(Debug)

include_directories(${PROJECT_SOURCE_DIR}/../ff/include)		
include_directories(${PROJECT_SOURCE_DIR}/utils)
SET(LIBS_LINK_DIRS ${PROJECT_SOURCE_DIR}/../bin)
LINK_DIRECTORIES(${LIBS_LINK_DIRS})
SET(LIBS ff pthread)

add_library(util utils/rrecord.cpp utils/parameter_parser.cpp)
SET(LIBS ${LIBS} util)

if(CACHE_EVAL)
  add_definitions(-DCACHE_EVAL)
  SET(LIBS ${LIBS} papi)
endif(CACHE_EVAL)

SET(LU_LIST LU/ff/lu_main.cpp)
ADD_EXECUTABLE(lu ${LU_LIST})  
TARGET_LINK_LIBRARIES(lu ${LIBS} )

SET(QS_LIST quick_sort/ff/quick_sort.cpp)
ADD_EXECUTABLE(quicksort ${QS_LIST})
TARGET_LINK_LIBRARIES(quicksort ${LIBS})

SET(CANNY_LIST canny/ff/run_canny.cpp
  canny/ff/canny_edge_detector.cpp)
find_package(wxWidgets)
if(wxWidgets_FOUND)
  include( "${wxWidgets_USE_FILE}")		
  ADD_EXECUTABLE(canny ${CANNY_LIST})
  TARGET_LINK_LIBRARIES(canny ${wxWidgets_LIBRARIES} ${LIBS})
else(wxWidgets_FOUND)
  message(WARNING "cannot find wxWidgets, ignore building canny")
endif (wxWidgets_FOUND)
#find_package(wxWidgets COMPONENTS core base REQUIRED)

SET(KMEANS_LIST kmeans/ff/main.cpp
  kmeans/ff/Lloyd.cpp
  kmeans/ff/point.cpp)
ADD_EXECUTABLE(kmeans ${KMEANS_LIST})
TARGET_LINK_LIBRARIES(kmeans ${LIBS}) 

SET(PBZIP_LIST pbzip/ff/main.cpp)
find_package (BZip2)
if(BZip2_FOUND)
  include_directories(${BZIP_INCLUDE_DIRS})
  ADD_EXECUTABLE(pbzip ${PBZIP_LIST})
  target_link_libraries(pbzip ${LIBS} ${BZIP2_LIBRARIES})  
else (BZip2_FOUND)
  message(WARNING "cannot find BZip2, ignore building pbzip")
endif (BZip2_FOUND)

SET(NQUEEN_LIST n_queen/ff/n_queen_ff.cpp)
ADD_EXECUTABLE(nqueen ${NQUEEN_LIST})
TARGET_LINK_LIBRARIES(nqueen ${LIBS}) 

SET(PHASH_LIST parallel_hash_insert/ff/main.cpp
  parallel_hash_insert/ff/HashTable.cpp)
ADD_EXECUTABLE(phash ${PHASH_LIST})
TARGET_LINK_LIBRARIES(phash ff pthread) 

SET(MUTEX_LIST mutex/ff/test_mutex.cpp)
ADD_EXECUTABLE(mutex ${MUTEX_LIST})
TARGET_LINK_LIBRARIES(mutex ff pthread) 

SET(FIB_LIST fibonacci/ff/fib.cpp)
ADD_EXECUTABLE(fib ${FIB_LIST})
TARGET_LINK_LIBRARIES(fib ${LIBS}) 
