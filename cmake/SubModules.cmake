
# EXTERNALS
# have ExternalProject available
include(ExternalProject)

set(SUBMODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries)

#OpenGL Stuff
include(submoduleGlBinding)

# Eigen
include(submoduleEigen3)

# Assimp
include(submoduleAssimp)
