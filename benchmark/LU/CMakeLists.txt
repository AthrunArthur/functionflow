
add_executable(lu_ff main.cpp ff/lu.cpp)
target_link_libraries(lu_ff ${FF_LIBS} ${COMMON_LIBS})

add_executable(lu_tbb main.cpp tbb/lu.cpp)
target_link_libraries(lu_tbb ${TBB_LIBS} ${COMMON_LIBS})

add_executable(lu_openmp main.cpp openmp/lu.cpp)
target_link_libraries(lu_openmp ${OPENMP_LIBS} ${COMMON_LIBS})

add_custom_target(lu)
add_dependencies(lu lu_ff lu_tbb lu_openmp)

add_dependencies(lu_ff util)
add_dependencies(lu_tbb util)
add_dependencies(lu_openmp util)

add_dependencies(FF lu_ff)
add_dependencies(TBB lu_tbb)
add_dependencies(OPENMP lu_openmp)
