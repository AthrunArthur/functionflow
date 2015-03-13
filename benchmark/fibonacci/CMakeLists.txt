set(FIB_LIST main.cpp)
set(FF_FIB_LIST main.cpp ff/fib.cpp)
set(TBB_FIB_LIST main.cpp tbb/fib.cpp)
set(OPENMP_FIB_LIST main.cpp openmp/fib_omp.cpp)

add_executable(fib_ff ${FF_FIB_LIST})
target_link_libraries(fib_ff ${FF_LIBS} ${COMMON_LIBS})

add_executable(fib_tbb ${TBB_FIB_LIST})
target_link_libraries(fib_tbb ${TBB_LIBS} ${COMMON_LIBS})

add_executable(fib_openmp ${OPENMP_FIB_LIST})
target_link_libraries(fib_openmp ${OPENMP_LIBS} ${COMMON_LIBS})

add_custom_target(fib)
add_dependencies(fib fib_ff fib_tbb fib_openmp)

add_dependencies(fib_ff util)
add_dependencies(fib_tbb util)
add_dependencies(fib_openmp util)

add_dependencies(FF fib_ff)
add_dependencies(TBB fib_tbb)
add_dependencies(OPENMP fib_openmp)
