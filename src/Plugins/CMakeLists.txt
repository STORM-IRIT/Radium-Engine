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
message("The following plugins will be compiled : ${SUBDIRS}")

set(libs
    radiumCore
    radiumEngine
    ${GLEW_LIBRARIES}
    ${Assimp_LIBRARIES}
    ${FREEIMAGE_LIBRARY}
    ${Qt5OpenGL_LIBRARIES}
    ${OPENGL_LIBRARIES})
    
foreach(subdir ${SUBDIRS})
    file(GLOB_RECURSE sources ${subdir}/*.cpp)
    file(GLOB_RECURSE headers ${subdir}/*.h ${subdir}/*.hpp)
    file(GLOB_RECURSE inlines ${subdir}/*.inl)
    
    add_library(
        ${subdir} SHARED
        ${sources}
        ${headers}
        ${inlines})
    
    set_target_properties(
        ${subdir}
        PROPERTIES
        VERSION 0.1.42)
    
    target_link_libraries(
        ${subdir}
        ${libs})
    
    install(
        TARGETS ${subdir}
        DESTINATION ${LIB_INSTALL_DIR})
    
endforeach()

set(plugins ${SUBDIRS})