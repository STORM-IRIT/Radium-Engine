
# here is defined the way we want to import glbinding
# TODO : verify with VERBOSE=1 on build time that CXX_FLAGS are correct
ExternalProject_Add(
        glbinding

        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/glbinding"

        # override default behaviours
        UPDATE_COMMAND ""

        # set the installatin to root
        # INSTALL_COMMAND cmake -E echo "Skipping install step."
        INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DOPTION_BUILD_TESTS=OFF
        -DOPTION_BUILD_GPU_TESTS=OFF
        -DOPTION_BUILD_DOCS=OFF
        -DOPTION_BUILD_TOOLS=OFF
        -DOPTION_BUILD_EXAMPLES=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_CXX_FLAGS=-D__has_feature\\\(x\\\)=false

        STEP_TARGETS build
        EXCLUDE_FROM_ALL TRUE
)


add_custom_target(glbinding_lib
        DEPENDS glbinding
        )
