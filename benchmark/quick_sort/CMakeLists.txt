add_executable(qsort_ff main.cpp ff/quick_sort.cpp)
target_link_libraries(qsort_ff ${FF_LIBS} ${COMMON_LIBS})

add_executable(qsort_tbb main.cpp tbb/quick_sort.cpp)
target_link_libraries(qsort_tbb ${TBB_LIBS} ${COMMON_LIBS})

add_executable(qsort_openmp main.cpp openmp/quick_sort.cpp)
target_link_libraries(qsort_openmp ${OPENMP_LIBS} ${COMMON_LIBS})

add_dependencies(qsort_ff util)
add_dependencies(qsort_tbb util)
add_dependencies(qsort_openmp util)

add_dependencies(FF qsort_ff)
add_dependencies(TBB qsort_tbb)
add_dependencies(OPENMP qsort_openmp)

