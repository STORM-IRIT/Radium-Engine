# -------------------------- Usefull fonctions for application and plugins client  ------------------------------------
# TODO : works well on MacOs : must be tested on other systems
include(CMakeParseArguments)
# Install resources : link (copy under window) the resource dir DIRECTORY into the buildtree at the given BUILDLOCATION and install the
# required files into the <bundle>/Resources
# If called only with a directory, all the files in this directory and subdirs will be installed
# to be called with
# installResources ( TARGET theTarget`
#                    [BUILDLOCATION whereToLinkInTheBuildTree]
#                    DIRECTORY theBaseRessourceDirectory
#                    [FILES [file1 [file2 ... ]]]
#
function(installTargetResources)
    # "declare" and parse parameters
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "TARGET;DIRECTORY;BUILDLOCATION"
            "FILES"
            ${ARGN}
    )
    # verify that the function was called with expected parameters
    if (NOT PARSED_ARGS_TARGET)
        message(FATAL_ERROR " [installResources] You must provide a target that need these resources")
    endif ()
    if (NOT PARSED_ARGS_DIRECTORY)
        message(FATAL_ERROR " [installResources] You must provide a resource directory")
    endif ()
    if (NOT PARSED_ARGS_BUILDLOCATION)
        # linking resours in the current bin dir of the build tree
        set(PARSED_ARGS_BUILDLOCATION ${CMAKE_CURRENT_BINARY_DIR})
    endif ()
    # compute resources dir for build tree and install tree
    get_filename_component(rsc_dir ${PARSED_ARGS_DIRECTORY} NAME)
    set(buildtree_dir ${PARSED_ARGS_BUILDLOCATION})
    # installing resources in the buildtree (link if available, copy if not)
    message(STATUS " [installResources] Linking resources directory ${PARSED_ARGS_DIRECTORY} for target ${PARSED_ARGS_TARGET} into ${buildtree_dir}/Resources/${rsc_dir}")
    file(MAKE_DIRECTORY "${buildtree_dir}")
    if (MSVC OR MSVC_IDE OR MINGW)
        add_custom_command(
                TARGET ${PARSED_ARGS_TARGET}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_directory ${PARSED_ARGS_DIRECTORY} "${buildtree_dir}/${rsc_dir}"
                VERBATIM
        )
    else ()
        add_custom_command(
                TARGET ${PARSED_ARGS_TARGET}
                POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E create_symlink ${PARSED_ARGS_DIRECTORY} "${buildtree_dir}/${rsc_dir}"
                VERBATIM
        )
    endif ()

    # Install in the install tree
    # Identify the individual files (to preserve directory structure)
    message(STATUS " [installResources] configuring install for requested files of ${PARSED_ARGS_DIRECTORY}")
    if (NOT PARSED_ARGS_FILES)
        file(GLOB_RECURSE PARSED_ARGS_FILES RELATIVE ${PARSED_ARGS_DIRECTORY} ${PARSED_ARGS_DIRECTORY}/*)
    endif ()
    foreach (file ${PARSED_ARGS_FILES})
        get_filename_component(file_dir ${file} DIRECTORY)
        if (APPLE)
            install(
                    FILES ${PARSED_ARGS_DIRECTORY}/${file}
                    DESTINATION ${PARSED_ARGS_TARGET}.app/Contents/Resources/${rsc_dir}/${file_dir}
            )
        else ()
            install(
                    FILES ${PARSED_ARGS_DIRECTORY}/${file}
                    DESTINATION Resources/${rsc_dir}/${file_dir}
            )
        endif ()
    endforeach ()
endfunction()
# --------------------------

# Configuration of the build and installation procedure for cmdline Radium application
# Allows to install application with dependent resources
# usage :
#   configure_xxx(
#         NAME theTargetName # <- this must be an executable
#         RESOURCES ResourceDir1 ResourceDir2 # <- acept a list of directories
# )

function(configure_cmdline_Radium_app)
    # "declare" and parse parameters
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "NAME"
            "RESOURCES" # list of directories containing the resources to install - optional
            ${ARGN}
    )
    if (NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR " [configure_radium_app] You must provide the main target of the application")
    endif ()
    # configure the application
    get_target_property(IsMacBundle ${PARSED_ARGS_NAME} MACOSX_BUNDLE)
    if (IsMacBundle)
        message(FATAL_ERROR " Error configuring ${PARSED_ARGS_NAME} as a cmdline application. A bundle was asked for this target.")
    endif ()
    # Configure the executable installation
    install(
            TARGETS ${PARSED_ARGS_NAME}
            RUNTIME DESTINATION bin
    )
    # install Radium Resources if there is a dependency on Engine or Guibase
    get_target_property(deps ${PARSED_ARGS_NAME} INTERFACE_LINK_LIBRARIES)
    list(FIND deps "Radium::RadiumEngine" depEngine)
    list(FIND deps "Radium::RadiumGuiBase" depGuibase)
    if (depEngine GREATER_EQUAL "0" OR depGuibase GREATER_EQUAL "0")
        install(DIRECTORY ${RADIUM_RESOURCES_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX})
    else ()
        message(STATUS " No Radium resource needed for ${PARSED_ARGS_NAME}")
    endif ()
    # Configure the resources installation
    if (PARSED_ARGS_RESOURCES)
        foreach (resLocation ${PARSED_ARGS_RESOURCES})
            message(STATUS " Installing resources ${resLocation} for ${PARSED_ARGS_NAME} ")
            installTargetResources(
                    TARGET ${PARSED_ARGS_NAME}
                    DIRECTORY ${resLocation}
                    BUILDLOCATION ${CMAKE_CURRENT_BINARY_DIR}/Resources
            )
        endforeach ()
    endif ()
endfunction()


function(configure_bundled_Radium_app)
    # "declare" and parse parameters
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "NAME"
            "RESOURCES" # list of directories containing the resources to install - optional
            ${ARGN}
    )
    if (NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR " [configure_radium_app] You must provide the main target of the application")
    endif ()
    # configure the application
    get_target_property(IsMacBundle ${PARSED_ARGS_NAME} MACOSX_BUNDLE)
    if (NOT IsMacBundle)
        message(FATAL_ERROR " Error configuring ${PARSED_ARGS_NAME} as a Bundled application. Only MacOsX is supported")
    endif ()

    set_target_properties(${PARSED_ARGS_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${PARSED_ARGS_NAME})
    set_target_properties(${PARSED_ARGS_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.radium.${PARSED_ARGS_NAME}")
    set_target_properties(${PARSED_ARGS_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    set_target_properties(${PARSED_ARGS_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

    # Configure the executable installation
    install(
            TARGETS ${PARSED_ARGS_NAME}
            BUNDLE DESTINATION "."
    )
    install(CODE "
        include(BundleUtilities)
        file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/${PARSED_ARGS_NAME}.app/Contents/Resources)
        file(COPY ${RADIUM_RESOURCES_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/${PARSED_ARGS_NAME}.app/Contents)
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/${PARSED_ARGS_NAME}.app \"\" \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\")
        "
            )

    # Configure the resources installation
    if (PARSED_ARGS_RESOURCES)
        foreach (resLocation ${PARSED_ARGS_RESOURCES})
            message(STATUS " Installing resources ${resLocation} for ${PARSED_ARGS_NAME} ")
            installTargetResources(
                    TARGET ${PARSED_ARGS_NAME}
                    DIRECTORY ${resLocation}
                    BUILDLOCATION ${CMAKE_CURRENT_BINARY_DIR}/${PARSED_ARGS_NAME}.app/Contents/Resources
            )
        endforeach ()
    endif ()
endfunction()

# Configuration of the build and installation procedure for Bundled Radium application
# Allows to install application with dependent resources
# usage :
#   configure_radium_app_install(
#         NAME theTargetName # <- this must be an executable
#         RESOURCES ResourceDir1 ResourceDir2 # <- acept a list of directories
# )
function(configure_radium_app_install)
    # "declare" and parse parameters
    cmake_parse_arguments(
            PARSED_ARGS
            ""
            "NAME"
            "RESOURCES" # list of directories containing the resources to install - optional
            ${ARGN}
    )
    if (NOT PARSED_ARGS_NAME)
        message(FATAL_ERROR " [configure_radium_app] You must provide the main target of the application")
    endif ()
    # configure the application
    get_target_property(IsMacBundle ${PARSED_ARGS_NAME} MACOSX_BUNDLE)
    if (IsMacBundle)
        message(STATUS " Configuring application ${PARSED_ARGS_NAME} as a MacOs Bundled application")
        configure_bundled_Radium_app(${ARGN})
    else ()
        message(STATUS " Configuring application ${PARSED_ARGS_NAME} as a command line application")
        configure_cmdline_Radium_app(${ARGN})
    endif ()
endfunction()
