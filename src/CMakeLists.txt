set(library_version "0.1.0")

# Find packages
find_package(OpenGL     REQUIRED)
find_package(Assimp     REQUIRED)
find_package(Qt5Core    REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL  REQUIRED)

include_directories(
 .
 ${EIGEN_INC}
 ${Assimp_INCLUDE_DIR}
)

# Build Core
set(core_target radiumCore)

file(GLOB_RECURSE core_sources Core/*.cpp)
file(GLOB_RECURSE core_headers Core/*.h Core/*.hpp)
file(GLOB_RECURSE core_inlines Core/*.inl)

set(core_libs ${OPENGL_LIBRARIES})

add_library(
 ${core_target} STATIC
 ${core_sources}
 ${core_headers}
 ${core_inlines}
)

SET_TARGET_PROPERTIES(
 ${core_target}
 PROPERTIES
 VERSION ${library_version}
)

target_link_libraries(
 ${core_target}
 ${core_libs}
)

INSTALL(
 TARGETS ${core_target}
 DESTINATION ${LIB_INSTALL_DIR}
)

# Build Engine
set(engine_target radiumEngine)

file(GLOB_RECURSE engine_sources Engine/*.cpp)
file(GLOB_RECURSE engine_headers Engine/*.h Engine/*.hpp)
file(GLOB_RECURSE engine_inlines Engine/*.inl)

file(GLOB_RECURSE engine_vert_glsl ../Shaders/*.vert.glsl)
file(GLOB_RECURSE engine_frag_glsl ../Shaders/*.frag.glsl)
file(GLOB_RECURSE engine_geom_glsl ../Shaders/*.geom.glsl)
file(GLOB_RECURSE engine_tesc_glsl ../Shaders/*.tesc.glsl)
file(GLOB_RECURSE engine_tese_glsl ../Shaders/*.tese.glsl)
file(GLOB_RECURSE engine_comp_glsl ../Shaders/*.comp.glsl)

set(engine_libs ${Assimp_LIBRARIES} ${Qt5OpenGL_LIBRARIES} ${OPENGL_LIBRARIES})

add_library(
 ${engine_target} STATIC
 ${engine_sources}
 ${engine_headers}
 ${engine_inlines}

 ${engine_vert_glsl}
 ${engine_frag_glsl}
 ${engine_geom_glsl}
 ${engine_tese_glsl}
 ${engine_tesc_glsl}
 ${engine_comp_glsl}
)

SET_TARGET_PROPERTIES(
 ${engine_target}
 PROPERTIES
 VERSION ${library_version}
)

target_link_libraries(
 ${engine_target}
 ${engine_libs}
)

INSTALL(
 TARGETS ${engine_target}
 DESTINATION ${LIB_INSTALL_DIR}
)

# Build MainApplication
set(app_target radium)

set(Qt5_LIBRARIES ${Qt5Core_LIBRARIES} ${Qt5Widgets_LIBRARIES} ${Qt5OpenGL_LIBRARIES})
set(app_libs radiumEngine radiumCore ${Qt5_LIBRARIES} ${OPENGL_LIBRARIES})

file(GLOB_RECURSE app_sources MainApplication/*.cpp)
file(GLOB_RECURSE app_headers MainApplication/*.h MainApplication/*.hpp)
file(GLOB_RECURSE app_inlines MainApplication/*.inl)

file(GLOB_RECURSE app_uis     MainApplication/*.ui)
qt5_wrap_ui(app_uis_moc ${app_uis})

# Qt5
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

# Fixme : Really usefull ?
add_definitions(${Qt5Core_DEFINITIONS})
add_definitions(${Qt5Widgets_DEFINITIONS})
add_definitions(${Qt5OpenGL_DEFINITIONS})

include_directories(
 .
 ${CMAKE_CURRENT_BINARY_DIR} # Moc
 ${EIGEN_INC}
)

add_executable(
 ${app_target}
 ${app_sources}
 ${app_headers}
 ${app_inlines}
 ${app_uis_moc}
)

target_link_libraries(
 ${app_target}
 ${app_libs}
)
