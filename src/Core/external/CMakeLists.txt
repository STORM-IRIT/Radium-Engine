cmake_minimum_required (VERSION 3.6)

project (radiumcore-external VERSION 1.0.0)

include(ExternalProject)
include(ExternalInclude)

#------------------------------------------------------------------------------
# Eigen - Linear Algebra library

# force installing by default all the external projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS install)

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

ExternalProject_Add(Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG e80ec243
    GIT_SHALLOW FALSE
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
    CMAKE_ARGS
    ${RADIUM_EXTERNAL_CMAKE_OPTIONS}
    -DEIGEN_TEST_CXX11=OFF
    -DBUILD_TESTING=OFF
    )

# I found some problems when generating xcode project for OpenMesh: the libXXX.dylib link to
# libXXX.Major.minor.dylib (eg libOpenMesh.2.1.dylib) is not generated and the script failed.
# Need to generate this link manually.
# TODO, find why only OpenMesh is problematic
ExternalProject_Add(OpenMesh
    GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
    GIT_TAG tags/OpenMesh-8.1
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
    CMAKE_ARGS
    ${RADIUM_EXTERNAL_CMAKE_OPTIONS}
    -DBUILD_APPS=OFF
    )

ExternalProject_Add(cpplocate
    GIT_REPOSITORY https://github.com/cginternals/cpplocate.git
    GIT_TAG tags/v2.2.0
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
    CMAKE_ARGS
    ${RADIUM_EXTERNAL_CMAKE_OPTIONS}
    -DOPTION_BUILD_TESTS=OFF
    -DOPTION_BUILD_DOCS=OFF
    )
