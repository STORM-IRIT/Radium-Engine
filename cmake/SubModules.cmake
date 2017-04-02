
# EXTERNALS
# have ExternalProject available
include(ExternalProject)

set(SUBMODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries)

#OpenGL Stuff
#add_subdirectory(${SUBMODULE_SOURCE_DIR}/OpenGL)
include(submoduleGlBinding)


# Eigen
set(EIGEN3_INCLUDE_DIR ${SUBMODULE_SOURCE_DIR}/Eigen)


# Assimp
# Set the external project assimp
include(submoduleAssimp)
