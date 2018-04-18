

file( GLOB_RECURSE stb_headers "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/stb/*.h"  )

add_custom_target(stb_lib
    COMMAND ${CMAKE_COMMAND} -E make_directory ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/stb/
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${stb_headers} ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/stb/
)

# ----------------------------------------------------------------------------------------------------------------------

set(STB_INCLUDE_DIR ${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/)
