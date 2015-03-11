SET(CANNY_LIST main.cpp
  canny_edge_detector.cpp)

SET(FF_CANNY_LIST ${CANNY_LIST} ff/run_canny.cpp)
SET(TBB_CANNY_LIST ${CANNY_LIST} tbb/run_canny.cpp)
SET(OPENMP_CANNY_LIST ${CANNY_LIST} openmp/run_canny.cpp)

find_package(wxWidgets)
if(wxWidgets_FOUND)
  include( "${wxWidgets_USE_FILE}")		
  ADD_EXECUTABLE(canny_ff ${FF_CANNY_LIST})
  TARGET_LINK_LIBRARIES(canny_ff ${wxWidgets_LIBRARIES} ${FF_LIBS} ${COMMON_LIBS})

  add_executable(canny_tbb ${TBB_CANNY_LIST})
  target_link_libraries(canny_tbb ${wxWidgets_LIBRARIES} ${TBB_LIBS} ${COMMON_LIBS})

  add_executable(canny_openmp ${OPENMP_CANNY_LIST})
  target_link_libraries(canny_openmp ${wxWidgets_LIBRARIES} ${COMMON_LIBS})
  add_dependencies(canny_ff util)
  add_dependencies(canny_tbb util)
  add_dependencies(canny_openmp util)

  add_dependencies(ff canny_ff)
  add_dependencies(tbb canny_tbb)
  add_dependencies(openmp canny_tbb)

else(wxWidgets_FOUND)
  message(WARNING "cannot find wxWidgets, ignore building canny")
endif (wxWidgets_FOUND)