set(src SerialSumTree.cpp
  main.cpp)

set(ff_src ${src} ff/ff_sum_tree.cpp)
#set(tbb_src ${src} tbb/opt_sum_tree.cpp)
set(tbb_src ${src} tbb/sum_tree.cpp)

add_executable(treesum_ff ${ff_src})
target_link_libraries(treesum_ff ${FF_LIBS} ${COMMON_LIBS})

add_executable(treesum_tbb ${tbb_src})
target_link_libraries(treesum_tbb ${TBB_LIBS} ${COMMON_LIBS})

add_custom_target(treesum)
add_dependencies(treesum treesum_ff treesum_tbb)

add_dependencies(treesum_ff util)
add_dependencies(treesum_tbb util)

add_dependencies(FF treesum_ff)
add_dependencies(TBB treesum_tbb)
