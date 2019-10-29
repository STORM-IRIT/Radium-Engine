cmake_minimum_required (VERSION 3.6)

project (radiumcore-external VERSION 1.0.0)

include(ExternalProject)

#------------------------------------------------------------------------------
# Eigen - Linear Algebra library

# force installing by default all the external projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS install)

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

ExternalProject_Add( Eigen3
    GIT_REPOSITORY https://github.com/eigenteam/eigen-git-mirror.git
    GIT_TAG origin/master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DEIGEN_TEST_CXX11=OFF
    -DBUILD_TESTING=OFF
    )

ExternalProject_Add( OpenMesh
    GIT_REPOSITORY https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
    GIT_TAG origin/master
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DBUILD_APPS=OFF
    )

ExternalProject_Add(cpplocate
        GIT_REPOSITORY https://github.com/cginternals/cpplocate.git
        GIT_TAG origin/master
        GIT_SHALLOW TRUE
        GIT_PROGRESS TRUE
        INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        -DOPTION_BUILD_TESTS=OFF
        -DOPTION_BUILD_DOCS=OFF
        )
