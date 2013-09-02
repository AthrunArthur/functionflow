SET(CANNY_LIST run_canny.cpp
		canny_edge_detector.cpp)

find_package(wxWidgets COMPONENTS core base REQUIRED)
include( "${wxWidgets_USE_FILE}")

INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

LINK_DIRECTORIES(${TOOLS_LINK_DIRS})
ADD_DEFINITIONS(${DEBUG_DEFS})

ADD_EXECUTABLE(canny ${CANNY_LIST})

TARGET_LINK_LIBRARIES(canny ${wxWidgets_LIBRARIES})
