
add_executable(nqueen_ff main.cpp ff/nqueen.cpp)
target_link_libraries(nqueen_ff ${FF_LIBS} ${COMMON_LIBS})

add_executable(nqueen_tbb main.cpp tbb/nqueen.cpp)
target_link_libraries(nqueen_tbb ${TBB_LIBS} ${COMMON_LIBS})

add_executable(nqueen_openmp main.cpp openmp/nqueen.cpp)
target_link_libraries(nqueen_openmp ${OPENMP_LIBS} ${COMMON_LIBS})

add_dependencies(nqueen_ff util)
add_dependencies(nqueen_tbb util)
add_dependencies(nqueen_openmp util)

add_dependencies(FF nqueen_ff)
add_dependencies(TBB nqueen_tbb)
add_dependencies(OPENMP nqueen_openmp)

