
add_executable(bitonic_ff main.cpp ff/bitonic.cpp)
target_link_libraries(bitonic_ff ${FF_LIBS} ${COMMON_LIBS})


add_custom_target(bitonic)
add_dependencies(bitonic bitonic_ff )#bitnoic_tbb bitnoic_openmp)


add_dependencies(FF bitonic_ff)

