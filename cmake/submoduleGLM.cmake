
# here is defined the way we want to import eigen
ExternalProject_Add(
        glm

        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/OpenGL/glm"

        # override default behaviours
        UPDATE_COMMAND ""

        # set the installatin to root
        # INSTALL_COMMAND cmake -E echo "Skipping install step."
        INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        STEP_TARGETS install
        EXCLUDE_FROM_ALL TRUE
)

add_custom_target(glm_lib
        DEPENDS glm
        )
# ----------------------------------------------------------------------------------------------------------------------

set(GLM_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/)
