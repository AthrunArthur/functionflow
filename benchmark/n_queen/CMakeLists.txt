SET(NQUEEN_LIST n_queen/ff/n_queen_ff.cpp)
ADD_EXECUTABLE(nqueen ${NQUEEN_LIST})
TARGET_LINK_LIBRARIES(nqueen ${LIBS}) 
