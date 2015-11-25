set( subdir "Animation" )

set(libs
    radiumCore
    radiumEngine
    FancyMesh
    ${GLEW_LIBRARIES}
    ${Assimp_LIBRARIES}
    ${FREEIMAGE_LIBRARY}
    ${Qt5OpenGL_LIBRARIES}
    ${OPENGL_LIBRARIES}
#    ${IBL_LIBRARY}
#    ${ISL_LIBRARY}
)

# Un-comment these lines to compile with the Implicit Plugin (that must be added via a symbolic link to the Plugins/ directory).
#set(IBL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../Radium-Engine-Plugins/ibl)
#set(IBL_INCLUDE_DIR ${IBL_DIR}/include)
#set(IBL_LIBRARY ${IBL_DIR}/libibl.a)
#include_directories(${IBL_INCLUDE_DIR})

#set(ISL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../../../Radium-Engine-Plugins/isl)
#set(ISL_INCLUDE_DIR ${ISL_DIR}/include)
#set(ISL_LIBRARY ${ISL_DIR}/libisl.a)
#include_directories(${ISL_INCLUDE_DIR})

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE headers *.h *.hpp)
file(GLOB_RECURSE inlines *.inl)

add_library(
    ${subdir} SHARED
    ${sources}
    ${headers}
    ${inlines})

target_compile_definitions(${subdir} PRIVATE "-DRA_CORE_IMPORTS")
target_compile_definitions(${subdir} PRIVATE "-DRA_ENGINE_IMPORTS")
target_compile_definitions(${subdir} PRIVATE "-D${subdir}_EXPORTS")

target_link_libraries(
    ${subdir}
    ${libs})

install(
    TARGETS ${subdir}
    DESTINATION ${LIB_INSTALL_DIR})
