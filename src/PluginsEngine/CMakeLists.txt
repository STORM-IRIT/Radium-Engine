macro(SUBDIRLIST result curdir)
  file(GLOB children RELATIVE ${curdir} ${curdir}/*)
  set(dirlist "")
  foreach(child ${children})
    if(IS_DIRECTORY ${curdir}/${child})
        list(APPEND dirlist ${child})
    endif()
  endforeach()
  set(${result} ${dirlist})
endmacro()

SUBDIRLIST(SUBDIRS ${CMAKE_CURRENT_SOURCE_DIR})
message("${CMAKE_CURRENT_SOURCE_DIR} - ${SUBDIRS}")

#set(engine_target radiumEngine)

#file(GLOB_RECURSE engine_sources Engine/*.cpp)
#file(GLOB_RECURSE engine_headers Engine/*.h Engine/*.hpp)
#file(GLOB_RECURSE engine_inlines Engine/*.inl)

#file(GLOB_RECURSE engine_vert_glsl ../Shaders/*.vert.glsl)
#file(GLOB_RECURSE engine_frag_glsl ../Shaders/*.frag.glsl)
#file(GLOB_RECURSE engine_geom_glsl ../Shaders/*.geom.glsl)
#file(GLOB_RECURSE engine_tesc_glsl ../Shaders/*.tesc.glsl)
#file(GLOB_RECURSE engine_tese_glsl ../Shaders/*.tese.glsl)
#file(GLOB_RECURSE engine_comp_glsl ../Shaders/*.comp.glsl)

#set(engine_libs
#    radiumCore
#    ${GLEW_LIBRARIES}
#    ${Assimp_LIBRARIES}
#    ${FREEIMAGE_LIBRARY}
#    ${Qt5OpenGL_LIBRARIES}
#    ${OPENGL_LIBRARIES})

#add_library(
#    ${engine_target} SHARED
#    ${engine_sources}
#    ${engine_headers}
#    ${engine_inlines}

#    ${engine_vert_glsl}
#    ${engine_frag_glsl}
#    ${engine_geom_glsl}
#    ${engine_tesc_glsl}
#    ${engine_tese_glsl}
#    ${engine_comp_glsl}
#)

#SET_TARGET_PROPERTIES(
#    ${engine_target}
#    PROPERTIES
#    VERSION ${library_version}
#)

#target_link_libraries(
#    ${engine_target}
#    ${engine_libs}
#)

#INSTALL(
#    TARGETS ${engine_target}
#    DESTINATION ${LIB_INSTALL_DIR}
#)
