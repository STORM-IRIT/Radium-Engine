
set ( tinyplylib_sources "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/tinyply/source/tinyply.cpp")
set ( tinyplylib_headers "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/tinyply/source/tinyply.h")

add_custom_target(tinyply
    COMMAND ${CMAKE_COMMAND} -E make_directory ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/tinyply/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${tinyplylib_headers} ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/tinyply/
)

# ----------------------------------------------------------------------------------------------------------------------

set(TINYPLY_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/)


