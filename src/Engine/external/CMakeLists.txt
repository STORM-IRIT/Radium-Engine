cmake_minimum_required (VERSION 3.6)

project (radiumengine-external VERSION 1.0.0)

include(ExternalProject)

#------------------------------------------------------------------------------
# Eigen - Linear Algebra library

# force installing by default all the external projects
set_property(DIRECTORY PROPERTY EP_STEP_TARGETS install)

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

ExternalProject_Add( glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG d162eee1e6f7c317a09229fe6ceab8ec6ab9a4b4
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DGLM_TEST_ENABLE=OFF
    -DBUILD_STATIC_LIBS=OFF
    )


ExternalProject_Add( glbinding
    GIT_REPOSITORY https://github.com/cginternals/glbinding.git
    GIT_TAG 28d32d9bbc72aedf815f18113b0bd3aa7b354108
    GIT_SHALLOW TRUE
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DOPTION_BUILD_TESTS=OFF
    -DOPTION_BUILD_DOCS=OFF
    -DOPTION_BUILD_TOOLS=OFF
    -DOPTION_BUILD_EXAMPLES=OFF
    )


ExternalProject_Add( globjects
    GIT_REPOSITORY https://github.com/cginternals/globjects.git
    GIT_TAG dc68b09a53ec20683d3b3a12ed8d9cb12602bb9a
    GIT_PROGRESS TRUE
    INSTALL_DIR "${CMAKE_INSTALL_PREFIX}"
        DEPENDS glbinding
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
    -DOPTION_BUILD_DOCS=OFF
    -DOPTION_BUILD_EXAMPLES=OFF
        -Dglbinding_DIR=${CMAKE_INSTALL_PREFIX}
    )

ExternalProject_Add( stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG 1034f5e5c4809ea0a7f4387e0cd37c5184de3cdd
    GIT_PROGRESS TRUE
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ${CMAKE_COMMAND} -E echo "Installing stb_image.h to ${CMAKE_INSTALL_PREFIX}/include"
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/include/stb/
      COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/stb_image.h ${CMAKE_INSTALL_PREFIX}/include/stb/
      COMMAND ${CMAKE_COMMAND} -E copy_if_different <SOURCE_DIR>/stb_image_write.h ${CMAKE_INSTALL_PREFIX}/include/stb/
    INSTALL_COMMAND ""
    )
