set(library_version "0.1.0")

set( CMAKE_SKIP_BUILD_RPATH FALSE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
set( CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

set( RADIUM_IO_IS_INTERFACE TRUE )

list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSysDir )
if ( "${isSysDir}" STREQUAL "-1" )
    set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
endif( "${isSysDir}" STREQUAL "-1" )

add_subdirectory( Core )
add_subdirectory( Engine )
add_subdirectory( IO )
add_subdirectory( GuiBase )

set(RADIUM_LIBRARIES ${RADIUM_LIBRARIES} radiumCore radiumEngine radiumIO radiumGuiBase PARENT_SCOPE)
