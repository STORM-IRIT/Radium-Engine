ExternalProject_Add(
    pbrt

    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/Pbrt"

    UPDATE_COMMAND ""

    INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
    CMAKE_GENERATOR ${CMAKE_GENERATOR}
    CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
    CMAKE_ARGS
    -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    -DINCLUDE_INSTALL_DIR=${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include
    -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
    -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
    -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
    -DCMAKE_CXX_FLAGS="-fPIC"
)

add_custom_target(pbrt_lib
    DEPENDS pbrt
)

set( PBRT_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/ )

file(
    COPY "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/Pbrt/src/"
    DESTINATION "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/pbrt"
)

if( APPLE )
    set( PBRT_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libpbrt.dylib" )
elseif ( UNIX )
    set( PBRT_LIBRARIES
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libglog.a"
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libHalf.a"
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libIex.a"
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libIexMath.a"
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libIlmImf.a"
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libIlmThread.a"
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libImath.a"
            "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libpbrt.a" )
elseif (MINGW)
    set( PBRT_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libpbrt.dll.a" )
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

    set(PBRT_LIBRARIES optimized "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/pbrt-${MSVC_PREFIX}-mt.lib")

endif()
