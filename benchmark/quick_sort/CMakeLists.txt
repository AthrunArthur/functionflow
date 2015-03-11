SET(QS_LIST quick_sort/ff/quick_sort.cpp)
ADD_EXECUTABLE(quicksort ${QS_LIST})
TARGET_LINK_LIBRARIES(quicksort ${LIBS})
