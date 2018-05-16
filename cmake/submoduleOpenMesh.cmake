if(${RADIUM_SUBMODULES_BUILD_TYPE} MATCHES Debug)
    set(OPENMESHLIBNAME OpenMeshCored)
else()
    set(OPENMESHLIBNAME OpenMeshCore)
endif()

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
    set(OPENMESH_DLL "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/${OPENMESHLIBNAME}.dll")
    set(OPENMESH_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/${OPENMESHLIBNAME}.lib")
endif()


if(MSVC OR MINGW)
  set( PLATFORM_ARGS "-DOPENMESH_BUILD_SHARED=TRUE" )
else()
  set( PLATFORM_ARGS "" )
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
        BUILD_BYPRODUCTS "${OPENMESH_LIBRARIES}"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DBUILD_APPS=OFF
        -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
        -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
        -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
        -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
        -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
        -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
        -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
        -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
        -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
        -DINSTALL_NAME_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} #override rpath to solve run bug on MACOSX
        ${PLATFORM_ARGS}
        EXCLUDE_FROM_ALL TRUE
)

add_custom_target(openmesh_lib
    DEPENDS openmesh
    )

if( MSVC OR MINGW )

        add_custom_target( openmesh_install_compiled_dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${OPENMESH_DLL} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
                COMMENT "copy openmesh dlls to bin dir" VERBATIM
            DEPENDS openmesh create_bin_dir
        )
        add_dependencies(openmesh_lib openmesh_install_compiled_dll)

endif()
