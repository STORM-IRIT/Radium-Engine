# Allow to compile with CLang
if ( ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang" )
    set( PLATFORM_ARGS "" )
else()
    set( PLATFORM_ARGS "-DCMAKE_CXX_FLAGS=-D__has_feature\\\(x\\\)=false" )
endif()

if(${RADIUM_SUBMODULES_BUILD_TYPE} MATCHES Debug)
    set(GLOBJECTLIBNAME globjectsd)
else()
    set(GLOBJECTLIBNAME globjects)
endif()

set(GLOBJECTS_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include)
if( APPLE )
    set( GLOBJECTS_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/lib${GLOBJECTLIBNAME}.dylib" )
elseif ( UNIX )
    set( GLOBJECTS_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/lib${GLOBJECTLIBNAME}.so")
elseif (MINGW)
    set( GLOBJECTS_DLL "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib${GLOBJECTLIBNAME}.dll")
    set( GLOBJECTS_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/lib${GLOBJECTLIBNAME}.dll.a" )
elseif( MSVC )
    set(GLOBJECTS_DLL "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/${GLOBJECTLIBNAME}.dll")
    set(GLOBJECTS_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/${GLOBJECTLIBNAME}.lib")
endif()

# here is defined the way we want to import globjects
ExternalProject_Add(
    globjects
    # Need to build glbinding_lib before configuring globjects
    DEPENDS glbinding_lib glm_lib

    # where the source will live
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/OpenGL/globjects"

    # override default behaviours
    UPDATE_COMMAND ""

    GIT_SUBMODULES 3rdPartyLibraries/OpenGL/globjects

    # set the installatin to root
    # INSTALL_COMMAND cmake -E echo "Skipping install step."
    INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
    BUILD_BYPRODUCTS "${GLOBJECTS_LIBRARIES}"
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY="${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/bin" # mandatory for dlls with MVSC
    -DOPTION_BUILD_TESTS=OFF
    -DOPTION_BUILD_DOCS=OFF
    -DOPTION_BUILD_EXAMPLES=OFF
    -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DGLM_INCLUDE_DIRS=${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include
    ${PLATFORM_ARGS}
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE
    )


add_custom_target(globjects_lib
    DEPENDS  glbinding_lib globjects
    )
# ----------------------------------------------------------------------------------------------------------------------

if( MSVC OR MINGW )

	add_custom_target( globjects_install_compiled_dll
                COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLOBJECTS_DLL} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMENT "copy globject dll to bin dir" VERBATIM
        DEPENDS globjects create_bin_dir
	)
	add_dependencies(globjects_lib globjects_install_compiled_dll)

endif()
