
set(src Lloyd.cpp
  main.cpp
  point.cpp)

set(ff_src ${src} ff/kmeans.cpp)
set(ru_ff_src ${src} ff/opt_kmeans.cpp)
set(tbb_src ${src} tbb/kmeans.cpp)
set(openmp_src ${src} openmp/kmeans.cpp)

add_executable(kmeans_ff ${ff_src})
target_link_libraries(kmeans_ff ${FF_LIBS} ${COMMON_LIBS})

add_executable(kmeans_ff_reuse ${ru_ff_src})
target_link_libraries(kmeans_ff_reuse ${FF_LIBS} ${COMMON_LIBS})

add_executable(kmeans_tbb ${tbb_src})
target_link_libraries(kmeans_tbb ${TBB_LIBS} ${COMMON_LIBS})

add_executable(kmeans_openmp ${openmp_src})
target_link_libraries(kmeans_openmp ${OPENMP_LIBS} ${COMMON_LIBS})

add_custom_target(kmeans)
add_dependencies(kmeans kmeans_ff kmeans_tbb kmeans_openmp kmeans_ff_reuse)

add_dependencies(kmeans_ff util)
add_dependencies(kmeans_tbb util)
add_dependencies(kmeans_openmp util)
add_dependencies(kmeans_ff_reuse util)

add_dependencies(FF kmeans_ff)
add_dependencies(FFREUSE kmeans_ff_reuse)
add_dependencies(TBB kmeans_tbb)
add_dependencies(OPENMP kmeans_openmp)
