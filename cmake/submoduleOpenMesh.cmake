set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

# ----------------------------------------------------------------------------------------------------------------------
set( OPENMESH_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include )
if( APPLE )
    set( OPENMESH_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libOpenMeshCore.dylib")
elseif ( UNIX )
    set( OPENMESH_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libOpenMeshCore.so")
elseif (MINGW)
    set( OPENMESH_DLL "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/libOpenMeshCore.dll")
    set( OPENMESH_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libOpenMeshCore.dll.a")
elseif( MSVC )
    # in order to prevent DLL hell, each of the DLLs have to be suffixed with the major version and msvc prefix
    if( MSVC70 OR MSVC71 )
        set(MSVC_PREFIX "vc70")
    elseif( MSVC80 )
        set(MSVC_PREFIX "vc80")
    elseif( MSVC90 )
        set(MSVC_PREFIX "vc90")
    elseif( MSVC10 )
        set(MSVC_PREFIX "vc100")
    elseif( MSVC11 )
        set(MSVC_PREFIX "vc110")
    elseif( MSVC12 )
        set(MSVC_PREFIX "vc120")
    else()
        set(MSVC_PREFIX "vc140")
    endif()
    set(OPENMESH_DLL "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libOpenMeshCore.dll")
    set(OPENMESH_LIBRARIES optimized "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/openmesh-${MSVC_PREFIX}-mt.lib")

endif()



# here is defined the way we want to import assimp
ExternalProject_Add(
        openmesh
        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/OpenMesh"

        # override default behaviours
        UPDATE_COMMAND ""

        GIT_SUBMODULES 3rdPartyLibraries/OpenMesh

        # set the installatin to installed/openmesh
        INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
        BUILD_BYPRODUCTS "${OPENMESH_LIBRARIES} ${OPENMESH_DLL}"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DBUILD_APPS=OFF
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DOPENMESH_BUILD_SHARED=TRUE
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
        EXCLUDE_FROM_ALL TRUE
)

add_custom_target(openmesh_lib
    DEPENDS openmesh
    )

if( MSVC OR MINGW )

        add_custom_target( openmesh_install_compiled_dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENMESH_DLL} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                COMMENT "copy openmesh dlls to bin dir" VERBATIM
		DEPENDS openmesh
	)
	add_dependencies(openmesh_lib openmesh_install_compiled_dll)

endif()
