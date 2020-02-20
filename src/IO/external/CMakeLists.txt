cmake_minimum_required (VERSION 3.6)

project (radiumio-external VERSION 1.0.0)

include(ExternalProject)

#------------------------------------------------------------------------------
# Eigen - Linear Algebra library

# force installing by default all the external projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS install)

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if (RADIUM_IO_ASSIMP)
    # I found some problems when generating xcode project for Assimp. The debug version is expected even in Release mode.
    # As Release build does not generate the debug version, the script fail.
    # For now, only configure in Debug mode to work with xcode
    ExternalProject_Add(assimp
            GIT_REPOSITORY https://github.com/assimp/assimp.git
            GIT_TAG origin/master
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
            INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
            CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DASSIMP_BUILD_ASSIMP_TOOLS=False
            -DASSIMP_BUILD_SAMPLES=False
            -DASSIMP_BUILD_TESTS=False
            -DIGNORE_GIT_HASH=True
            -DASSIMP_NO_EXPORT=True
            )
endif()


if(RADIUM_IO_TINYPLY)
    ExternalProject_Add( tinyply
        GIT_REPOSITORY https://github.com/ddiakopoulos/tinyply.git
        GIT_TAG origin/master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DCMAKE_MACOSX_RPATH=TRUE
        -DSHARED_LIB=TRUE
        )
endif()
