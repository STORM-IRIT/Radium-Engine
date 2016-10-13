# Settings
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH         ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# Painty project
# Radium
set( RADIUM "${CMAKE_CURRENT_SOURCE_DIR}/../" )

# Locate the lib to use later
set( RADIUM_LIB_DIR "${RADIUM}/lib" )
set( RADIUM_LIB_SRC "${RADIUM}/src" )
# TODO(hugo) maybe let the radium's cmakelist do that

set( RADIUM_LIB_GUIBASE "${RADIUM_LIB_DIR}/libradiumGuiBase.so" )
set( RADIUM_LIB_ENGINE  "${RADIUM_LIB_DIR}/libradiumEngine.so"  )
set( RADIUM_LIB_CORE    "${RADIUM_LIB_DIR}/libradiumCore.so"    )
# TODO(hugo) this should be OS-aware, because windows won't work
#            very well with .so ...
# Includes
set( RADIUM_INC_EIGEN  "${RADIUM}/3rdPartyLibraries/Eigen" )
set( RADIUM_INC_ASSIMP "${RADIUM}/3rdPartyLibraries/Assimp/include" )

# Add Painty source code

add_subdirectory(MainApplication)
add_subdirectory(Test0)
