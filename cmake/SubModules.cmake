
# EXTERNALS
# have ExternalProject available
include(ExternalProject)

set(SUBMODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries)
set(RADIUM_SUBMODULES_INSTALL_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/3rdPartyLibraries)
set(RADIUM_SUBMODULES_BUILD_TYPE Release)

#OpenGL Stuff
#add_subdirectory(${SUBMODULE_SOURCE_DIR}/OpenGL)
include(submoduleGlBinding)


# Eigen
include(submoduleEigen3)

# Assimp
# Set the external project assimp
include(submoduleAssimp)
