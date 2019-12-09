# installLibHeaders "Core" "Utils/foo.h" -> install in include/Core/Utils/foo.h
# You must pass both parameters using quotes
macro( installLibHeaders TARGET_DIR_NAME FILES )
    foreach ( file ${FILES} )
        get_filename_component( file_dir ${file} DIRECTORY )
        install( FILES ${file} DESTINATION include/${TARGET_DIR_NAME}/${file_dir} )
    endforeach()
endmacro()

# install resources : link the resource dir into the buildtree and install the required files into the <bundle>/Resources
# If called only with a directory, all the files in this directory and subdirs will be installed
# to be called with
# installResources ( TARGET theTarget
#                    DIRECTORY theBaseRessourceDirectory
#                    [FILES [file1 [file2 ... ]]]
#
include(CMakeParseArguments)
function(installResources)
    # "declare" and parse parameters
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "TARGET;DIRECTORY"
            "FILES"
            ${ARGN}
    )
    # verify that the function was called with expected parameters
    if (NOT PARSED_ARGS_DIRECTORY)
        message(FATAL_ERROR " [installResources] You must provide a resource directory")
    endif ()
    # compute resources dir for build tree and install tree
    get_filename_component(rsc_dir ${PARSED_ARGS_DIRECTORY} NAME)
    get_filename_component(buildtree_dir ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY)
    #
    # installing resources in the buildtree (link if available, copy if not)
    message(STATUS " [installResources] Linking resources directory ${PARSED_ARGS_DIRECTORY} for target ${PARSED_ARGS_TARGET} into ${buildtree_dir}/Resources/${rsc_dir}")
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../Resources")
    if (${CMAKE_VERSION} VERSION_GREATER_EQUAL 10.2) # 3.14)
        # Warning, this will be executed only at configure time (to be verified)
        file(CREATE_LINK ${PARSED_ARGS_DIRECTORY} "${buildtree_dir}/Resources/${rsc_dir}" COPY_ON_ERROR SYMBOLIC)
    else ()
        if (MSVC OR MSVC_IDE OR MINGW)
            add_custom_command(
                    TARGET ${PARSED_ARGS_TARGET}
                    PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${PARSED_ARGS_DIRECTORY} "${buildtree_dir}/Resources/${rsc_dir}"
                    VERBATIM
            )
        else ()
            add_custom_command(
                    TARGET ${PARSED_ARGS_TARGET}
                    PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E create_symlink ${PARSED_ARGS_DIRECTORY} "${buildtree_dir}/Resources/${rsc_dir}"
                    VERBATIM
            )
        endif ()
    endif ()
    # Install in the install tree
    # Identify the individual files (to preserve directory structure)
    if (NOT PARSED_ARGS_FILES)
        file(GLOB_RECURSE PARSED_ARGS_FILES RELATIVE ${PARSED_ARGS_DIRECTORY} ${PARSED_ARGS_DIRECTORY}/*)
    endif ()
    message(STATUS " [installResources] Installing files ${PARSED_ARGS_FILES}")
    foreach (file ${PARSED_ARGS_FILES})
        get_filename_component( file_dir ${file} DIRECTORY )
        install(FILES ${PARSED_ARGS_DIRECTORY}/${file} DESTINATION Resources/${rsc_dir}/${file_dir})
    endforeach()
endfunction(installResources)
