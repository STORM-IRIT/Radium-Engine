

if( MSCV )
    # In order to avoid a bug in glm CMakelist, leading to invalid
        # install path for glmConfig.cmake, we simply copy the glm
        # library and use the custom FindGLM.cmake as fallback
    set ( glm_headers "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/OpenGL/glm/glm")

    add_custom_target(glm
        FILE (COPY ${glm_headers} DESTINATION ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include)
    )
else ()
# here is defined the way we want to import glm
ExternalProject_Add(
        glm

        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/OpenGL/glm"

        # override default behaviours
        UPDATE_COMMAND ""

        GIT_SUBMODULES 3rdPartyLibraries/OpenGL/glm
        # set the installatin to root
        # INSTALL_COMMAND cmake -E echo "Skipping install step."
        INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
        -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
        -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        STEP_TARGETS install
        EXCLUDE_FROM_ALL TRUE
)

endif()

add_custom_target(glm_lib
        DEPENDS glm
        )
# ----------------------------------------------------------------------------------------------------------------------

set(GLM_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/)
