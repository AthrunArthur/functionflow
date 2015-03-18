SET(CANNY_LIST main.cpp
  canny_edge_detector.cpp)

SET(FF_CANNY_LIST ${CANNY_LIST} ff/opt_run_canny.cpp)
SET(FF_NORMAL_LIST ${CANNY_LIST} ff/normal_run_canny.cpp)
set(FF_REUSE_LIST ${CANNY_LIST} ff/opt_run_canny.cpp)
SET(TBB_CANNY_LIST ${CANNY_LIST} tbb/run_canny.cpp)
SET(OPENMP_CANNY_LIST ${CANNY_LIST} openmp/run_canny.cpp)
set(CMAKE_BINARY_DIR ${PROJECT_BINARY_DIR}/../)

find_package(wxWidgets)
if(wxWidgets_FOUND)
  include( "${wxWidgets_USE_FILE}")		
  ADD_EXECUTABLE(canny_ff ${FF_CANNY_LIST})
  TARGET_LINK_LIBRARIES(canny_ff ${wxWidgets_LIBRARIES} ${FF_LIBS} ${COMMON_LIBS})
  
  add_executable(canny_ff_reuse ${FF_REUSE_LIST})
  target_link_libraries(canny_ff_reuse ${wxWidgets_LIBRARIES} ${FF_LIBS} ${COMMON_LIBS})
  
  add_executable(canny_ff_normal ${FF_NORMAL_LIST})
  target_link_libraries(canny_ff_normal ${wxWidgets_LIBRARIES} ${FF_LIBS} ${COMMON_LIBS})

  add_executable(canny_tbb ${TBB_CANNY_LIST})
  target_link_libraries(canny_tbb ${wxWidgets_LIBRARIES} ${TBB_LIBS} ${COMMON_LIBS})

  add_executable(canny_openmp ${OPENMP_CANNY_LIST})
  target_link_libraries(canny_openmp ${wxWidgets_LIBRARIES} ${OPENMP_LIBS} ${COMMON_LIBS})

  add_custom_target(canny)
  add_dependencies(canny canny_ff canny_tbb canny_openmp canny_ff_reuse canny_ff_normal)

  add_dependencies(canny_ff util)
  add_dependencies(canny_tbb util)
  add_dependencies(canny_openmp util)
  add_dependencies(canny_ff_reuse util)
  add_dependencies(canny_ff_normal util)

  add_dependencies(FF canny_ff)
  add_dependencies(FFREUSE canny_ff_reuse)
  add_dependencies(TBB canny_tbb)
  add_dependencies(OPENMP canny_tbb)

else(wxWidgets_FOUND)
  message(WARNING "cannot find wxWidgets, ignore building canny")
endif (wxWidgets_FOUND)
