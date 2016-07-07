project(AnimationPlugin)

cmake_minimum_required(VERSION 2.8.11)
if ( NOT PLUGIN_OUTPUT_PATH )
  set ( PLUGIN_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/Plugins/bin )
else()
  message( "Output path : ${PLUGIN_OUTPUT_PATH}" )
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )
set(EXECUTABLE_OUTPUT_PATH         ${PLUGIN_OUTPUT_PATH} )
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PLUGIN_OUTPUT_PATH} )

if ( NOT CMAKE_PREFIX_PATH )
  set( CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR} )
endif()

# Win32 stuff
if (MSVC OR MSVC_IDE)
    # Copy libs / targets in the correct directories
    if ("${CMAKE_GENERATOR}" STREQUAL "NMake Makefiles")
        set(PDB_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
    else()
        foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
            string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
            set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PLUGIN_OUTPUT_PATH})
            set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PLUGIN_OUTPUT_PATH})
            set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${PLUGIN_OUTPUT_PATH})
        endforeach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    endif()
endif(MSVC OR MSVC_IDE)

find_package( Qt5Core    REQUIRED )
find_package( Qt5Widgets REQUIRED )

file( GLOB_RECURSE sources src/*.cpp )
file( GLOB_RECURSE headers src/*.hpp )
file( GLOB_RECURSE inlines src/*.inl )

file(GLOB_RECURSE app_uis  src/*.ui)
qt5_wrap_ui(app_uis_moc ${app_uis})

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_definitions( ${Qt5Core_DEFINITIONS} )
add_definitions( ${Qt5Widges_DEFINITIONS} )

include_directories(
    src/
    ${CMAKE_CURRENT_BINARY_DIR} # MOC
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INC}
    ${ASSIMP_INC}
    )

add_library( Animation SHARED ${sources} ${headers} ${inlines} ${app_uis})
add_dependencies( Animation radiumCore radiumEngine )

target_compile_definitions( Animation PRIVATE "-DAnimation_EXPORTS" )

set(Qt5_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
target_link_libraries( Animation
                       ${Qt5_LIBRARIES}
                       ${RADIUM_LIBRARIES}
                     )


