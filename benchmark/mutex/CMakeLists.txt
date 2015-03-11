SET(MUTEX_LIST mutex/ff/test_mutex.cpp)
ADD_EXECUTABLE(mutex ${MUTEX_LIST})
TARGET_LINK_LIBRARIES(mutex ff pthread) 
