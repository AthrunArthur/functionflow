SET(KMEANS_LIST kmeans/ff/main.cpp
  kmeans/ff/Lloyd.cpp
  kmeans/ff/point.cpp)
ADD_EXECUTABLE(kmeans ${KMEANS_LIST})
TARGET_LINK_LIBRARIES(kmeans ${LIBS}) 
