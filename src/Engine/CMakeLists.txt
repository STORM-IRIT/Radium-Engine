# Find packages
find_package(OpenGL     REQUIRED)
find_package(Qt5 COMPONENTS OpenGL REQUIRED)

if(Qt5Core_VERSION VERSION_LESS 5.5 OR
   Qt5Widgets_VERSION VERSION_LESS 5.5 OR
   Qt5OpenGL_VERSION VERSION_LESS 5.5)
    message(FATAL_ERROR "Qt5 or superior required (found ${Qt5Core_VERSION}).")
else ()
    message(STATUS "QT ${Qt5Core_VERSION} found." )
endif()

# Build Engine
set(engine_target radiumEngine)

file(GLOB_RECURSE engine_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE engine_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE engine_inlines ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

file(GLOB_RECURSE engine_glsl ${CMAKE_CURRENT_SOURCE_DIR}/../Shaders/*.glsl)

set( engine_shaders ${engine_glsl})

set(engine_libs
    radiumCore
    Qt5::OpenGL
    ${OPENGL_LIBRARIES}
    ${GLBINDING_LIBRARIES}
    ${GLOBJECTS_LIBRARIES}
   )

add_library(
    ${engine_target} SHARED
    ${engine_sources}
    ${engine_headers}
    ${engine_inlines}
    ${engine_shaders}
)

target_include_directories(${engine_target}
    PRIVATE
           ${CMAKE_CURRENT_SOURCE_DIR}/..
    PUBLIC 
           ${GLBINDING_INCLUDE_DIR}
           ${GLOBJECTS_INCLUDE_DIR}
           ${GLM_INCLUDE_DIR}
    INTERFACE 
           ${STB_INCLUDE_DIR}
    )

add_dependencies( ${engine_target} radiumCore glbinding_lib globjects_lib glm_lib stb_lib)

target_compile_definitions(${engine_target} PRIVATE "-DRA_ENGINE_EXPORTS")


#This one should be extracted directly from external project properties.
target_compile_definitions(${engine_target}
        PUBLIC
        GLOBJECTS_USE_EIGEN
)

target_link_libraries(
    ${engine_target}
    PUBLIC ${engine_libs}
)

set_property( TARGET ${engine_target} PROPERTY POSITION_INDEPENDENT_CODE ON )
