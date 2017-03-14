# Do not clean OpenGL submodules libs when cleaning the build.
# set_directory_properties(PROPERTIES CLEAN_NO_CUSTOM 1)

include(FindPackageHandleStandardArgs)


#do not compile submodules tools, example and doc
option(OPTION_BUILD_TESTS     "Build tests."                                           OFF)
option(OPTION_BUILD_GPU_TESTS "Build tests that require an OpenGL context."            OFF)
option(OPTION_BUILD_DOCS      "Build documentation."                                   OFF)
option(OPTION_BUILD_TOOLS     "Build tools."                                           OFF)
option(OPTION_BUILD_EXAMPLES  "Build examples."                                        OFF)


set(GLSUBMODILES_INSTALL_PREFIX ${SUBMODULES_OUTPUT_DIRECTORY}/glsubmodules)

# pb when compiling ContextHandle
#define __has_feature(x) false
add_definitions(-D__has_feature\\\(x\\\)=false)


# set default locations for openGL submodules
# variable can bet set to other directory. If not set, consider variables refer to the
# OpenGL submodules included in the Radium source distribution
# TODO : verify if the if(DEFINED ...) are really mandatory ...

# cpplocate
#if(DEFINED cpplocate_DIR)
#else(DEFINED cpplocate_DIR)
#    set(cpplocate_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cpplocate)
#    message("Opengl Submodules : cpplocate DIR is ${cpplocate_DIR}")
#    #add_subdirectory(${cpplocate_DIR})
#endif(DEFINED cpplocate_DIR)

# glbinding
if(DEFINED glbinding_DIR)
else(DEFINED glbinding_DIR)
    set(glbinding_DIR ${CMAKE_CURRENT_SOURCE_DIR}/glbinding)
    #message("Opengl Submodules : glbinding DIR is ${glbinding_DIR}")
    # configure glbinding submodule
    add_subdirectory(${glbinding_DIR})

    # taken from http://stackoverflow.com/questions/40294146/alternative-to-cmake-post-build-command-when-target-is-in-subdirectory?rq=1
    # make copied file be dependent from one which is build.
    # Note, that DEPENDS here creates dependencies both from the target
    # and from the file it creates.
    add_custom_command(OUTPUT ${GLSUBMODILES_INSTALL_PREFIX}/glbinding}
            #create directory
            COMMAND ${CMAKE_COMMAND} -E make_directory ${GLSUBMODILES_INSTALL_PREFIX}/lib
            COMMAND ${CMAKE_COMMAND} -E make_directory ${GLSUBMODILES_INSTALL_PREFIX}/include
            #copy lib files
            #COMMAND echo "$<TARGET_FILE_NAME:glbinding> ... $<TARGET_SONAME_FILE_NAME:glbinding> +++ $<TARGET_LINKER_FILE_NAME:glbinding> ---  "
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:glbinding> ${GLSUBMODILES_INSTALL_PREFIX}/lib
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_FILE_NAME:glbinding> ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_SONAME_FILE_NAME:glbinding>
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_SONAME_FILE_NAME:glbinding> ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_LINKER_FILE_NAME:glbinding>
            #copy include files
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/glbinding/source/glbinding/include/glbinding ${GLSUBMODILES_INSTALL_PREFIX}/include/glbinding
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/glbinding/source/glbinding/include/glbinding ${GLSUBMODILES_INSTALL_PREFIX}/include/glbinding
            DEPENDS glbinding
            )
    # Create target which consume the command via DEPENDS.
    add_custom_target(copy_glbinding_files ALL
            DEPENDS ${GLSUBMODILES_INSTALL_PREFIX}/glbinding}
            )
endif(DEFINED glbinding_DIR)

find_package(GLM REQUIRED)

#globjects
if(DEFINED globjects_DIR)
else(DEFINED globjects_DIR)
    set(globjects_DIR ${CMAKE_CURRENT_SOURCE_DIR}/globjects)
    #message("Opengl Submodules : globjects DIR is ${globjects_DIR}")

    set_target_properties(pack component_install
            PROPERTIES OUTPUT_NAME pack_globject
            OUTPUT_NAME component_install_globject)

    # configure globjects submodule
    add_subdirectory(${globjects_DIR})


    # taken from http://stackoverflow.com/questions/40294146/alternative-to-cmake-post-build-command-when-target-is-in-subdirectory?rq=1
    # make copied file be dependent from one which is build.
    # Note, that DEPENDS here creates dependencies both from the target
    # and from the file it creates.
    add_custom_command(OUTPUT ${GLSUBMODILES_INSTALL_PREFIX}/globjects}
            #create directory
            COMMAND ${CMAKE_COMMAND} -E make_directory ${GLSUBMODILES_INSTALL_PREFIX}/lib
            COMMAND ${CMAKE_COMMAND} -E make_directory ${GLSUBMODILES_INSTALL_PREFIX}/include
            #copy lib files
            #COMMAND echo "$<TARGET_FILE_NAME:glbinding> ... $<TARGET_SONAME_FILE_NAME:glbinding> +++ $<TARGET_LINKER_FILE_NAME:glbinding> ---  "
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:globjects> ${GLSUBMODILES_INSTALL_PREFIX}/lib
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_FILE_NAME:globjects> ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_SONAME_FILE_NAME:globjects>
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_SONAME_FILE_NAME:globjects> ${GLSUBMODILES_INSTALL_PREFIX}/lib/$<TARGET_LINKER_FILE_NAME:globjects>
            #copy include files
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/globjects/source/globjects/include/globjects ${GLSUBMODILES_INSTALL_PREFIX}/include/globjects
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_BINARY_DIR}/globjects/source/globjects/include/globjects ${GLSUBMODILES_INSTALL_PREFIX}/include/globjects
            DEPENDS glbinding
            )
    # Create target which consume the command via DEPENDS.
    add_custom_target(copy_globjects_files ALL
            DEPENDS ${GLSUBMODILES_INSTALL_PREFIX}/globjects}
            )
endif(DEFINED globjects_DIR)


