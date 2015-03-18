
add_executable(quad_ff main.cpp ff/quad.cpp)
target_link_libraries(quad_ff ${FF_LIBS} ${COMMON_LIBS})

add_executable(quad_tbb main.cpp tbb/quad.cpp)
target_link_libraries(quad_tbb ${TBB_LIBS} ${COMMON_LIBS})

add_custom_target(quad)
add_dependencies(quad quad_ff quad_tbb)

add_dependencies(quad_ff util)
add_dependencies(quad_tbb util)

add_dependencies(FF quad_ff)
add_dependencies(TBB quad_tbb)

