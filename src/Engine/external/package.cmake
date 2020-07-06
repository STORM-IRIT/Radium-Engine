set(glm_sub_DIR lib/${CMAKE_LIBRARY_ARCHITECTURE}/cmake/glm/ CACHE INTERNAL "")
set(glbinding_sub_DIR share/glbinding/ CACHE INTERNAL "")
set(globjects_sub_DIR share/globjects/ CACHE INTERNAL "")
set(Eigen3_sub_DIR share/eigen3/cmake/ CACHE INTERNAL "")

set(glm_DIR ${CMAKE_INSTALL_PREFIX}/${glm_sub_DIR})
set(glbinding_DIR ${CMAKE_INSTALL_PREFIX}/${glbinding_sub_DIR})
set(globjects_DIR ${CMAKE_INSTALL_PREFIX}/${globjects_sub_DIR})
set(Eigen3_DIR ${CMAKE_INSTALL_PREFIX}/${Eigen3_sub_DIR})

