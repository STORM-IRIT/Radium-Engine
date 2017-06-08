# here is defined the way we want to import assimp
ExternalProject_Add(
        assimp
        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/Assimp"

        # override default behaviours
        UPDATE_COMMAND ""

        # set the installatin to installed/assimp
        INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DASSIMP_BUILD_ASSIMP_TOOLS=False
            -DASSIMP_BUILD_SAMPLES=False
            -DASSIMP_BUILD_TESTS=False
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
            -DASSIMP_INSTALL_PDB=False
)

add_custom_target(assimp_lib
    DEPENDS assimp
    )

# ----------------------------------------------------------------------------------------------------------------------
set( ASSIMP_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include )
if( APPLE )
    set( ASSIMP_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libassimp.dylib" )
elseif ( UNIX )
    set( ASSIMP_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libassimp.so" )
elseif (MINGW)
    set( ASSIMP_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libassimp.dll.a" )
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

    set(ASSIMP_LIBRARIES optimized "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/assimp-${MSVC_PREFIX}-mt.lib")

endif()
