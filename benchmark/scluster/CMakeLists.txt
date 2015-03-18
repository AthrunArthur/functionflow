set(src Lloyd.cpp
  main.cpp
  point.cpp)

set(ff_src ${src} ff/scluster.cpp)
set(ru_ff_src ${src} ff/opt_scluster.cpp)

add_executable(scluster_ff_normal ${ff_src})
target_link_libraries(scluster_ff_normal ${FF_LIBS} ${COMMON_LIBS})

add_executable(scluster_ff_reuse ${ru_ff_src})
target_link_libraries(scluster_ff_reuse ${FF_LIBS} ${COMMON_LIBS})

add_custom_target(scluster)
add_dependencies(scluster scluster_ff_normal scluster_ff_reuse)

add_dependencies(scluster_ff_normal util)
add_dependencies(scluster_ff_reuse util)

add_dependencies(FF scluster_ff_normal)
add_dependencies(FFREUSE scluster_ff_reuse)
