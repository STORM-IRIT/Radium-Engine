
# here is defined the way we want to import glbinding
ExternalProject_Add(
    glbinding

    # where the source will live
    SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/OpenGL/glbinding"

    # override default behaviours
    UPDATE_COMMAND ""

    # set the installatin to root
    # INSTALL_COMMAND cmake -E echo "Skipping install step."
    INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
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
    -DCMAKE_CXX_FLAGS=-D__has_feature\\\(x\\\)=false
    STEP_TARGETS build
    EXCLUDE_FROM_ALL TRUE
    )


add_custom_target(glbinding_lib
    DEPENDS glbinding
    )
# ----------------------------------------------------------------------------------------------------------------------

set(GLBINDING_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include)
if( APPLE )
    set( GLBINDING_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libglbinding.dylib" )
elseif ( UNIX )
    set( GLBINDING_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libglbinding.so")
elseif (MINGW)
    set( GLBINDING_LIBRARIES "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/libglbinding.dll.a" )
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

    set(GLBINDING_LIBRARIES optimized "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/lib/glbinding.lib")

endif()
