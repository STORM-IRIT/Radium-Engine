
# EXTERNALS
# have ExternalProject available
include(ExternalProject)

set(SUBMODULE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries)
set(RADIUM_SUBMODULES_INSTALL_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/3rdPartyLibraries)
set(RADIUM_SUBMODULES_BUILD_TYPE Release)

if (MSVC OR MSVC_IDE)
        set(RADIUM_SUBMODULES_INSTALL_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/${CMAKE_BUILD_TYPE}/3rdPartyLibraries)
        set(RADIUM_SUBMODULES_BUILD_TYPE ${CMAKE_BUILD_TYPE})
else()
        set(RADIUM_SUBMODULES_INSTALL_DIRECTORY ${RADIUM_BUNDLE_DIRECTORY}/3rdPartyLibraries)
        set(RADIUM_SUBMODULES_BUILD_TYPE Release)
endif()

# ----------------------------------------------------------------------------------------------------------------------
if( MSVC OR MINGW )

    add_custom_target( create_bin_dir
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
        COMMENT "Force generation of bin directory" VERBATIM
    )

endif()

#OpenGL Stuff
include(submoduleGLM)
include(submoduleGlBinding)
include(submoduleGlObjects)

# Eigen
include(submoduleEigen3)

# Assimp
if (RADIUM_ASSIMP_SUPPORT)
    include(submoduleAssimp)
endif()

# OpenMesh
include(submoduleOpenMesh)

# TinyPly
if (RADIUM_TINYPLY_SUPPORT)
    include(submoduleTinyPly)
endif()
