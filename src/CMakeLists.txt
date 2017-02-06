set(library_version "0.1.0")

set( CMAKE_SKIP_BUILD_RPATH FALSE )
set( CMAKE_BUILD_WITH_INSTALL_RPATH FALSE )
set( CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib )
set( CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

list( FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSysDir )
if ( "${isSysDir}" STREQUAL "-1" )
    set( CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib" )
endif( "${isSysDir}" STREQUAL "-1" )

# Find packages
find_package(OpenGL     REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)

if (MSVC OR MSVC_IDE OR MINGW)
  find_package(GLEW       REQUIRED) #only used on windows builds.
endif (MSVC OR MSVC_IDE OR MINGW)

include_directories(
    .
    ${EIGEN_INC}
    ${PATATE_INC}
    ${ASSIMP_INC}
)

# Build Core
set(core_target radiumCore)

include(CMakeGenerateVersion.txt)

file(GLOB_RECURSE core_sources Core/*.cpp)
file(GLOB_RECURSE core_headers Core/*.h Core/*.hpp)
file(GLOB_RECURSE core_inlines Core/*.inl)

set(core_libs ${OPENGL_LIBRARIES})

add_library(
    ${core_target} SHARED
    ${core_sources}
    ${core_headers}
    ${core_inlines}
)

target_compile_definitions(${core_target} PRIVATE "-DRA_CORE_EXPORTS")

target_link_libraries(
    ${core_target}
    ${core_libs}
)

# Build Engine
set(engine_target radiumEngine)

file(GLOB_RECURSE engine_sources Engine/*.cpp)
file(GLOB_RECURSE engine_headers Engine/*.h Engine/*.hpp)
file(GLOB_RECURSE engine_inlines Engine/*.inl)

file(GLOB_RECURSE engine_glsl ../Shaders/*.glsl)

set( engine_shaders ${engine_glsl})

set(engine_libs
    radiumCore
    ${GLEW_LIBRARIES}
    ${ASSIMP_LIB}
    ${Qt5OpenGL_LIBRARIES}
    ${OPENGL_LIBRARIES})

add_library(
    ${engine_target} SHARED
    ${engine_sources}
    ${engine_headers}
    ${engine_inlines}

    ${engine_shaders}
)

add_dependencies( ${engine_target} ${core_target} assimp )

target_compile_definitions(${engine_target} PRIVATE "-DRA_ENGINE_EXPORTS")

target_link_libraries(
    ${engine_target}
    ${engine_libs}
)


# Build GuiBase
set(guibase_target radiumGuiBase)

# Qt5
set(Qt5_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Fixme : Really usefull ?
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

file(GLOB_RECURSE guibase_sources GuiBase/*.cpp)
file(GLOB_RECURSE guibase_headers GuiBase/*.h guibase/*.hpp)
file(GLOB_RECURSE guibase_inlines GuiBase/*.inl)

file(GLOB_RECURSE plugin_base PluginBase/*.hpp)

set (guibase_libs
    radiumEngine
    radiumCore
    ${Qt5_LIBRARIES}
)

add_library(
    ${guibase_target} SHARED
    ${guibase_sources}
    ${guibase_headers}
    ${guibase_inlines}
    ${plugin_base}
)

add_dependencies( ${guibase_target} ${guibase_target} ${core_target} )
target_compile_definitions(${guibase_target} PRIVATE "-DRA_GUIBASE_EXPORTS")

target_link_libraries(
    ${guibase_target}
    ${guibase_libs}
)

# Build tests
add_subdirectory(Tests)
