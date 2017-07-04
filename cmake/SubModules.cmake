
# EXTERNALS
# have ExternalProject available
include(ExternalProject)

set(SUBMODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries)

if (MSVC OR MSVC_IDE)
	set(RADIUM_SUBMODULES_INSTALL_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/3rdPartyLibraries)
	set(RADIUM_SUBMODULES_BUILD_TYPE ${CMAKE_BUILD_TYPE})
else()
	set(RADIUM_SUBMODULES_INSTALL_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/3rdPartyLibraries)
	set(RADIUM_SUBMODULES_BUILD_TYPE Release)
endif()


#OpenGL Stuff
include(submoduleGLM)
include(submoduleGlBinding)
include(submoduleGlObjects)

# Eigen
include(submoduleEigen3)

# Assimp
include(submoduleAssimp)
