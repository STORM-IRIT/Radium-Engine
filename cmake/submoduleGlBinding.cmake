# Allow to compile with CLang
if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
    set( PLATFORM_ARGS "" )
else()
    set( PLATFORM_ARGS "-DCMAKE_CXX_FLAGS=-D__has_feature\\\(x\\\)=false" )
endif()

if(${RADIUM_SUBMODULES_BUILD_TYPE} MATCHES Debug)
    set(GLBINDINGLIBNAME glbindingd)
else()
    set(GLBINDINGLIBNAME glbinding)
endif()

set(GLBINDING_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include)
if( APPLE )
    set( GLBINDING_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/lib${GLBINDINGLIBNAME}.dylib" )
elseif ( UNIX )
    set( GLBINDING_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/lib${GLBINDINGLIBNAME}.so")
elseif (MINGW)
    set(GLBINDING_DLL            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib${GLBINDINGLIBNAME}.dll")
    set( GLBINDING_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/lib${GLBINDINGLIBNAME}.dll.a" )
elseif( MSVC )
    set(GLBINDING_DLL "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/${GLBINDINGLIBNAME}.dll")
    set(GLBINDING_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/${GLBINDINGLIBNAME}.lib")
endif()


# here is defined the way we want to import glbinding
ExternalProject_Add(
    glbinding

    # where the source will live
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/OpenGL/glbinding"

    # override default behaviours
    UPDATE_COMMAND ""

    GIT_SUBMODULES 3rdPartyLibraries/OpenGL/glbinding
    # set the installatin to root
    # INSTALL_COMMAND cmake -E echo "Skipping install step."
    INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
    BUILD_BYPRODUCTS "${GLBINDING_LIBRARIES}"
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/bin" # mandatory for dlls with MVSC
    -DOPTION_BUILD_TESTS=OFF
    -DOPTION_BUILD_GPU_TESTS=OFF
    -DOPTION_BUILD_DOCS=OFF
    -DOPTION_BUILD_TOOLS=OFF
    -DOPTION_BUILD_EXAMPLES=OFF
    -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    ${PLATFORM_ARGS}
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE
    )


add_custom_target(glbinding_lib
    DEPENDS glbinding
    )
# ----------------------------------------------------------------------------------------------------------------------
if( MSVC OR MINGW )

    add_custom_target( glbinding_install_compiled_dll
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLBINDING_DLL} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMENT "copy glbinding dll to bin dir" VERBATIM
        DEPENDS glbinding create_bin_dir
	)
    add_dependencies(glbinding_lib glbinding_install_compiled_dll)

endif()
