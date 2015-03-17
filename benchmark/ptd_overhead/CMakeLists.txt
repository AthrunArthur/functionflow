
add_executable(ptd_base main.cpp func_do_base.cpp)
target_link_libraries(ptd_base ${FF_LIBS} ${COMMON_LIBS})

add_executable(ptd_cmp main.cpp func_do_cmp.cpp)
target_link_libraries(ptd_cmp ${FF_LIBS} ${COMMON_LIBS})

add_custom_target(ptd)
add_dependencies(ptd ptd_base ptd_cmp)

add_dependencies(ptd_base util)
add_dependencies(ptd_cmp util)
