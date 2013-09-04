# SET(LU_LIST LU/ff/LU_resolve.cpp)
SET(QS_LIST quick_sort/ff/quick_sort.cpp)
# SET(RANDOM LU/ff/gen_matrix.cpp)
SET(CANNY_LIST canny/ff/run_canny.cpp
		canny/ff/canny_edge_detector.cpp)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include( "${wxWidgets_USE_FILE}")		
		
INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

LINK_DIRECTORIES(${TOOLS_LINK_DIRS})
ADD_DEFINITIONS(${DEBUG_DEFS})
add_definitions(-O2)

# ADD_EXECUTABLE(lu ${LU_LIST})
ADD_EXECUTABLE(quicksort ${QS_LIST})
# ADD_EXECUTABLE(random ${RANDOM})
ADD_EXECUTABLE(canny ${CANNY_LIST})

# TARGET_LINK_LIBRARIES(lu ${TOOLS_LINK_LIBS} ff pthread)
TARGET_LINK_LIBRARIES(quicksort ${TOOLS_LINK_LIBS} ff pthread)
# TARGET_LINK_LIBRARIES(random ${TOOLS_LINK_LIBS} ff pthread)
TARGET_LINK_LIBRARIES(canny ${wxWidgets_LIBRARIES} ${TOOLS_LINK_LIBS} ff pthread)