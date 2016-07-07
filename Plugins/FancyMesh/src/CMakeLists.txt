set( subdir "FancyMesh" )

set(libs
    radiumCore
    radiumEngine
    ${GLEW_LIBRARIES}
    ${Assimp_LIBRARIES}
    ${FREEIMAGE_LIBRARY}
    ${Qt5OpenGL_LIBRARIES}
    ${OPENGL_LIBRARIES}
)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE headers *.h *.hpp)
file(GLOB_RECURSE inlines *.inl)

add_library(
    ${subdir} SHARED
    ${sources}
    ${headers}
    ${inlines})

target_compile_definitions(${subdir} PRIVATE "-D${subdir}_EXPORTS")

target_link_libraries(
    ${subdir}
    ${libs})
