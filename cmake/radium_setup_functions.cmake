#-- -- -- -- -- -- -- -- --  Usefull fonctions for application and plugins client -- -- -- -- -- -- -- -- -- -- -- --
#CMAKE tips : To delay evaluation of some variable to build time, just escape their name( e.g \${NAME} instead of ${NAME}
#TODO : works well on MacOs : must be tested on other systems
#TODO, some part are only for MACOS but could be generalized to all systems
#   (MACOSX_BUNDLE on MacOs, Bundle like directories on other systems

# Minimum version:
# 3.13: install target from different directory
#       see https://github.com/STORM-IRIT/Radium-Engine/pull/550#issuecomment-637415860
cmake_minimum_required(VERSION 3.13 FATAL_ERROR)

if (MSVC OR MSVC_IDE OR MINGW)
    include( Windeployqt )
endif()

include(CMakeParseArguments)
# Introduction of two customs properties in the buildchain
# these properties allow identify dependency resources when configuring/installing a target.
define_property(TARGET
    PROPERTY RADIUM_TARGET_RESOURCES_DIRECTORY
    BRIEF_DOCS "Identify the optional resource directory associated with a target."
    FULL_DOCS "Contains a directory that will be linked or installed when building or installing the target."
    )
define_property(TARGET
    PROPERTY RADIUM_TARGET_RESOURCES_FILES
    BRIEF_DOCS "Identify the optional resource files associated with a target."
    FULL_DOCS "Contains a list of individual files that will be installed when installing the target."
    )

# ------------------------------------------------------------------------------
# Internal functions not to be called directly by the user.
#    Radium client might prefer to use main entry points.
#

# Configuration of the build and installation procedure for cmdline Radium application
# Allows to install application with dependent resources
# usage :
#   configure_cmdline_Radium_app(
#         NAME theTargetName # <- this must be an executable
#         RESOURCES ResourceDir1 ResourceDir2 # <- acept a list of directories
# )
function(configure_cmdline_Radium_app)
    # "declare" and parse parameters
    cmake_parse_arguments(
        ARGS
        ""
        "NAME"
        "RESOURCES" # list of directories containing the resources to install - optional
        ${ARGN}
    )
    if (NOT ARGS_NAME)
        message(FATAL_ERROR " [configure_radium_app] You must provide the main target of the application")
    endif ()
    # configure the application
    if (APPLE)
        get_target_property(IsMacBundle ${ARGS_NAME} MACOSX_BUNDLE)
        if (IsMacBundle)
            message(FATAL_ERROR " Error configuring ${ARGS_NAME} as a cmdline application. A bundle was asked for this target.")
        endif ()
    endif ()
    # Configure the executable installation
    install(
        TARGETS ${ARGS_NAME}
        RUNTIME DESTINATION bin
    )
    # TODO, this Windows only bundle fix might be adapted to Linux also ... perhaps with the same code ?
    if (MSVC OR MSVC_IDE OR MINGW)
        # Construciton of the  dependency paths
        set(FIX_LIBRARY_DIR "${CMAKE_INSTALL_PREFIX}")
        # Add the Qt bin dir ...
        list(APPEND FIX_LIBRARY_DIR "${QtDlls_location}")
        # Fix the bundled directory
        install(CODE "
                        message(STATUS \"Fixing application with Qt base direcory at ${FIX_LIBRARY_DIR} !!\")
            include(BundleUtilities)
                        fixup_bundle( ${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.exe \"\" \"${FIX_LIBRARY_DIR}\")
            "
            )
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            install(FILES $<TARGET_PDB_FILE:${ARGS_NAME}> DESTINATION bin)
        endif()
    endif ()

    # Configure the application own resources installation
    if (ARGS_RESOURCES)
        foreach (resLocation ${ARGS_RESOURCES})
            message(STATUS " Installing resources ${resLocation} for ${ARGS_NAME} ")
            installTargetResources(
                TARGET ${ARGS_NAME}
                DIRECTORY ${resLocation}
                BUILDLOCATION ${CMAKE_CURRENT_BINARY_DIR}/Resources
            )
        endforeach ()
    endif ()
endfunction()


# NOTE that only MacosX is supported for now to handle bundled applications
# But the script could be generalized to all systems by integrfating the bundle structure
# (MACOSX_BUNDLE* and BundleName.app are explicitely used here)
#
# Configuration of the build and installation procedure for bundled Radium application
# Allows to install application with dependent resources
# usage :
#   configure_bundled_Radium_app(
#         NAME theTargetName # <- this must be an executable
#         RESOURCES ResourceDir1 ResourceDir2 # <- accept a list of directories
#         [USE_PLUGIN] # set this option if Plugins from Radium bundle must be imported in thea applicaiton bundle
# )
function(configure_bundled_Radium_app)
    # "declare" and parse parameters
    cmake_parse_arguments(
        ARGS
        "USE_PLUGINS"
        "NAME"
        "RESOURCES" # list of directories containing the resources to install - optional
        ${ARGN}
    )
    if (NOT ARGS_NAME)
        message(FATAL_ERROR " [configure_radium_app] You must provide the main target of the application")
    endif ()
    # configure the application
    get_target_property(IsMacBundle ${ARGS_NAME} MACOSX_BUNDLE)
    if (NOT IsMacBundle)
        message(FATAL_ERROR " Error configuring ${ARGS_NAME} as a Bundled application. Only MacOsX is supported")
    endif ()

    set_target_properties(${ARGS_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_NAME ${ARGS_NAME})
    set_target_properties(${ARGS_NAME} PROPERTIES MACOSX_BUNDLE_BUNDLE_GUI_IDENTIFIER "com.radium.${ARGS_NAME}")
    set_target_properties(${ARGS_NAME} PROPERTIES MACOSX_BUNDLE_LONG_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")
    set_target_properties(${ARGS_NAME} PROPERTIES MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")

    # Configure the executable installation
    install(
        TARGETS ${ARGS_NAME}
        BUNDLE DESTINATION "bin/"
    )
    if (ARGS_USE_PLUGINS)
        install(CODE "
        include(BundleUtilities)
        set(BU_CHMOD_BUNDLE_ITEMS TRUE)
        file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app/Contents/Resources)
        file(COPY ${RADIUM_RESOURCES_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app/Contents)
        if (EXISTS ${RADIUM_PLUGINS_DIR})
            file(COPY ${RADIUM_PLUGINS_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app/Contents/)
        else()
            file(MAKE_DIRECTORY \"${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app/Contents/Plugins/lib\")
        endif()
        file(GLOB RadiumAvailablePlugins
                RELATIVE ${RADIUM_PLUGINS_DIR}/lib/
                ${RADIUM_PLUGINS_DIR}/lib/*.dylib )
        set(InstalledPlugins)
        foreach (plugin \${RadiumAvailablePlugins})
            list( APPEND InstalledPlugins ${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app/Contents/Plugins/lib/\${plugin} )
        endforeach ()
        fixup_bundle(${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app \"\${InstalledPlugins}\" \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\")
        "
            )
    else ()
        install(CODE "
            message(STATUS \"Installing ${ARGS_NAME} without plugins\")
            include(BundleUtilities)
            set(BU_CHMOD_BUNDLE_ITEMS TRUE)
            file(REMOVE_RECURSE ${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app/Contents/Resources)
            file(COPY ${RADIUM_RESOURCES_DIR} DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app/Contents)
            fixup_bundle(${CMAKE_INSTALL_PREFIX}/bin/${ARGS_NAME}.app \"\" \"${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR}\")
            "
            )
    endif ()

    # Configure the resources installation
    if (ARGS_RESOURCES)
        foreach (resLocation ${ARGS_RESOURCES})
            message(STATUS " Installing resources ${resLocation} for ${ARGS_NAME} ")
            installTargetResources(
                TARGET ${ARGS_NAME}
                DIRECTORY ${resLocation}
                BUILDLOCATION ${CMAKE_CURRENT_BINARY_DIR}/bin/${ARGS_NAME}.app/Contents/Resources
            )
        endforeach ()
    endif ()
endfunction()

# Install plugin resources
# Not the same than target resources because of the destination directory (here, the Radium 'Bundle'
function(installPluginResources)
    # "declare" and parse parameters
    cmake_parse_arguments(
        ARGS
        ""
        "TARGET;DIRECTORY;BUILD_LOCATION;INSTALL_LOCATION"
        "FILES"
        ${ARGN}
    )
    # verify that the function was called with expected parameters
    if (NOT ARGS_TARGET)
        message(FATAL_ERROR " [installPluginResources] You must provide a target that need these resources")
    endif ()
    if (NOT ARGS_DIRECTORY)
        message(FATAL_ERROR " [installPluginResources] You must provide a resource directory")
    endif ()
    if (NOT ARGS_BUILD_LOCATION)
        # linking resources in the current bin dir of the build tree
        set(ARGS_BUILD_LOCATION ${CMAKE_CURRENT_BINARY_DIR})
    endif ()
    if (NOT ARGS_INSTALL_LOCATION)
        # installing in the default install dir
        set(ARGS_INSTALL_LOCATION ${CMAKE_INSTALL_PREFIX})
    endif ()
    # compute resources dir for build tree and install tree
    get_filename_component(rsc_dir ${ARGS_DIRECTORY} NAME)
    set(buildtree_dir ${ARGS_BUILD_LOCATION})
    # installing resources in the buildtree (link if available, copy if not)
    message(STATUS " [installPluginResources] Linking resources directory ${ARGS_DIRECTORY} for target ${ARGS_TARGET} into ${buildtree_dir}/${rsc_dir}")
    file(MAKE_DIRECTORY "${buildtree_dir}")
    if (MSVC OR MSVC_IDE OR MINGW)
        add_custom_command(
            TARGET ${ARGS_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARGS_DIRECTORY} "${buildtree_dir}/${rsc_dir}"
            VERBATIM
        )
    else ()
        add_custom_command(
            TARGET ${ARGS_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${ARGS_DIRECTORY} "${buildtree_dir}/${rsc_dir}"
            VERBATIM
        )
    endif ()

    # Install in the Radium install tree
    # Identify the individual files (to preserve directory structure)
    message(STATUS " [installResources] configuring install for requested files of ${ARGS_DIRECTORY}")
    if (NOT ARGS_FILES)
        file(GLOB_RECURSE ARGS_FILES RELATIVE ${ARGS_DIRECTORY} ${ARGS_DIRECTORY}/*)
    endif ()
    foreach (file ${ARGS_FILES})
        get_filename_component(file_dir ${file} DIRECTORY)
        install(
            FILES ${ARGS_DIRECTORY}/${file}
            DESTINATION ${ARGS_INSTALL_LOCATION}/Resources/${ARGS_TARGET}/${rsc_dir}/${file_dir}
        )
    endforeach ()
endfunction()
# ------------------------------------------------------------------------------

# Install resources for the target  theTarget:
#    - Link( copy under window ) the resource dir DIRECTORY into the buildtree at the given BUILD_LOCATION
#    - Install the required files into the TARGET_INSTALL_DIR/Resources[/PREFIX] directory
# If called only with a directory, all the files from this directory and subdirs will be installed
# This function also define the custom properties RADIUM_RESOURCE_DIRECTORY and RADIUM_RESOURCE_FILES
# for the given target with the corresponding value.
# To be called with
# installResources( TARGET theTarget`
#                  DIRECTORY theBaseResourceDirectory
#                  [BUILD_LOCATION whereToLinkInTheBuildTree]
#                  [PREFIX TargetResourcePathPrefix]
#                  [FILES[file1[file2...]]]
function(installTargetResources)
    #"declare" and parse parameters
    cmake_parse_arguments(
        ARGS
        ""
        "TARGET;DIRECTORY;BUILD_LOCATION;PREFIX"
        "FILES"
        ${ARGN}
    )
    #verify that the function was called with expected parameters
    if (NOT ARGS_TARGET)
        message(FATAL_ERROR " [installResources] You must provide a target that need these resources")
    endif ()
    if (NOT ARGS_DIRECTORY)
        message(FATAL_ERROR " [installResources] You must provide a resource directory")
    endif ()
    if (NOT ARGS_BUILD_LOCATION)
        # linking resources as in Radium bundle build dir : ${CMAKE_CURRENT_BINARY_DIR}/../Resources
        get_filename_component(resourceBaseDir ${CMAKE_CURRENT_BINARY_DIR} DIRECTORY)
        set(ARGS_BUILD_LOCATION ${resourceBaseDir}/Resources)
    endif ()

    #compute resources dir for build tree and install tree
    get_filename_component(rsc_dir ${ARGS_DIRECTORY} NAME)
    set(buildtree_dir ${ARGS_BUILD_LOCATION})
    #installing resources in the buildtree( link if available, copy if not)
    message(STATUS " [installResources] Linking resources directory ${ARGS_DIRECTORY} for target ${ARGS_TARGET} into ${buildtree_dir}/${rsc_dir}")
    file(MAKE_DIRECTORY "${buildtree_dir}")
    if (MSVC OR MSVC_IDE OR MINGW)
        add_custom_command(
            TARGET ${ARGS_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory ${ARGS_DIRECTORY} "${buildtree_dir}/${rsc_dir}"
            VERBATIM
        )
    else ()
        add_custom_command(
            TARGET ${ARGS_TARGET}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E create_symlink ${ARGS_DIRECTORY} "${buildtree_dir}/${rsc_dir}"
            VERBATIM
        )
    endif ()
    #Install in the install tree
    #Identify the individual files( to preserve directory structure )
    # set the target properties
    message(STATUS " [installTargetResources] Set resources properties for target ${ARGS_TARGET} to ${ARGS_DIRECTORY}")
    set_target_properties(${ARGS_TARGET}
        PROPERTIES
        RADIUM_TARGET_RESOURCES_DIRECTORY ${ARGS_DIRECTORY}
        )
    if (ARGS_FILES)
        set_target_properties(${ARGS_TARGET}
            PROPERTIES
            RADIUM_TARGET_RESOURCES_FILES ${ARGS_FILES}
            )
    else ()
        file(GLOB_RECURSE ARGS_FILES RELATIVE ${ARGS_DIRECTORY} ${ARGS_DIRECTORY}/*)
    endif ()
    if (ARGS_PREFIX)
        set(ARGS_PREFIX "${ARGS_PREFIX}/")
    endif ()
    #install resource for application or shared library
    get_target_property(targetType ${ARGS_TARGET} TYPE)

    if (${targetType} STREQUAL "EXECUTABLE")
        foreach (file ${ARGS_FILES})
            get_filename_component(file_dir ${file} DIRECTORY)
            if (APPLE)
                install(
                    FILES ${ARGS_DIRECTORY}/${file}
                    DESTINATION ${ARGS_TARGET}.app/Contents/Resources/${rsc_dir}/${file_dir}
                )
            else ()
                install(
                    FILES ${ARGS_DIRECTORY}/${file}
                    DESTINATION Resources/${rsc_dir}/${file_dir}
                )
            endif ()
        endforeach ()
    elseif (${targetType} STREQUAL "SHARED_LIBRARY")
        message(STATUS " [installTargetResources] Installing resources for target ${ARGS_TARGET} into Resources/${ARGS_PREFIX}${rsc_dir}/${file_dir}")
        foreach (file ${ARGS_FILES})
            get_filename_component(file_dir ${file} DIRECTORY)
            install(
                FILES ${ARGS_DIRECTORY}/${file}
                DESTINATION Resources/${ARGS_PREFIX}${rsc_dir}/${file_dir}
            )
        endforeach ()
    else ()
        message(FATAL_ERROR "Unknonw target type ${targetType} for target ${ARGS_TARGET} ")
    endif ()
endfunction()


# Configuration of the build and installation procedure for Radium application
# Allows to install application with dependent resources
# usage :
#   configure_radium_app(
#         NAME theTargetName # <- this must be an executable
#         [USE_PLUGINS] # <- The application uses Radium Plugins : install available plugins into the bundle is it is one
#         [RESOURCES ResourceDir1 ResourceDir2] # <- acept a list of directories
# )
function(configure_radium_app)
    # "declare" and parse parameters
    cmake_parse_arguments(
        ARGS
        "USE_PLUGINS"
        "NAME"
        "RESOURCES" # list of directories containing the resources to install - optional
        ${ARGN}
    )
    if (NOT ARGS_NAME)
        message(FATAL_ERROR " [configure_radium_app] You must provide the main target of the application")
    endif ()
    # configure the application
    if (APPLE)
        get_target_property(IsMacBundle ${ARGS_NAME} MACOSX_BUNDLE)
        if (IsMacBundle)
            message(STATUS " Configuring application ${ARGS_NAME} as a MacOs Bundled application")
            configure_bundled_Radium_app(${ARGN})
        else ()
            message(STATUS " Configuring application ${ARGS_NAME} as a command line application")
            configure_cmdline_Radium_app(${ARGN})
        endif ()
    else ()
        message(STATUS " Configuring application ${ARGS_NAME} as a command line application")
        configure_cmdline_Radium_app(${ARGN})
    endif ()

    get_target_property(IsUsingQt ${ARGS_NAME} LINK_LIBRARIES)
    list(FIND IsUsingQt "Qt5::Core" QTCOREIDX)
    if(NOT QTCOREIDX EQUAL -1)
        if (MSVC OR MSVC_IDE OR MINGW)
            message(STATUS " Preparing call to WinDeployQT for application ${ARGS_NAME}")
            windeployqt( ${ARGS_NAME} bin )
        endif ()
    endif ()
endfunction()

# ---------------------------------------------------------------------------------------------

# Configuration of the build and installation procedure for a Radium plugin
# Allows to install plugin with dependent resources and to import them in applications.
# Import by the application can be done at runtime from the Radium installation directory
# or, for bundled application, at install time by adding the available plugins into the bundle
# usage :
#   configure_radium_plugin(
#         NAME theTargetName # <- this must be a plugin
#         RESOURCES ResourceDir1 ResourceDir2 # <- accept a list of directories
# )
function(configure_radium_plugin)
    # "declare" and parse parameters
    cmake_parse_arguments(
        ARGS
        "INSTALL_IN_RADIUM_BUNDLE"
        "NAME"
        "RESOURCES;HELPER_LIBS" # list of directories containing the resources to install - optional
        ${ARGN}
    )
    if (NOT ARGS_NAME)
        message(FATAL_ERROR " [configure_radium_plugin] You must provide the main target of the plugin")
    endif ()
    set_target_properties(${ARGS_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Plugins/lib)

    if( CMAKE_BUILD_TYPE MATCHES Debug )
        message(STATUS " [configure_radium_plugin] Plugin compiled with debug info")
        target_compile_definitions(${ARGS_NAME} PUBLIC PLUGIN_IS_COMPILED_WITH_DEBUG_INFO)
        file( COPY "${RADIUM_ROOT_DIR}/lib/cmake/Radium/pluginMetaDataDebug.json" DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
        target_sources(${ARGS_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/pluginMetaDataDebug.json")
    else()
        file( COPY "${RADIUM_ROOT_DIR}/lib/cmake/Radium/pluginMetaDataRelease.json" DESTINATION ${CMAKE_CURRENT_BINARY_DIR} )
        target_sources(${ARGS_NAME} PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/pluginMetaDataRelease.json")
    endif()

    # configure the plugin installation
    # a plugin could be installed in the Radium Installation Directory
    if (ARGS_INSTALL_IN_RADIUM_BUNDLE)
        set(${ARGS_NAME}_INSTALL_DIR "${RADIUM_ROOT_DIR}/Plugins")
    else ()
        set(${ARGS_NAME}_INSTALL_DIR ${CMAKE_INSTALL_PREFIX})
    endif ()
    install(
        TARGETS ${ARGS_NAME}
        DESTINATION ${${ARGS_NAME}_INSTALL_DIR}
        LIBRARY DESTINATION ${${ARGS_NAME}_INSTALL_DIR}/lib
    )
    # Configure the plugin helper library
    if (ARGS_HELPER_LIBS)
        # Adding rpath @loader_path to the library
        set_target_properties(${ARGS_NAME}
            PROPERTIES INSTALL_RPATH "@loader_path;${CMAKE_INSTALL_RPATH}")
        foreach (helperLib ${ARGS_HELPER_LIBS})
            message(STATUS " [configure_plugin] Request to install the helper library ${helperLib} for plugin ${ARGS_NAME}")
            get_target_property(resourceDir ${helperLib} RADIUM_TARGET_RESOURCES_DIRECTORY)
            if (NOT ${resourceDir} STREQUAL "resourceDir-NOTFOUND")
                message(STATUS " [configure_plugin]    Installing ${helperLib} resources in ${${ARGS_NAME}_INSTALL_DIR}/Resources/${helperLib}/${rsc_dir}/${file_dir}")
                get_filename_component(rsc_dir ${resourceDir} NAME)
                file(GLOB_RECURSE RSC_FILES RELATIVE ${resourceDir} ${resourceDir}/*)
                foreach (file ${RSC_FILES})
                    get_filename_component(file_dir ${file} DIRECTORY)
                    install(
                        FILES ${resourceDir}/${file}
                        DESTINATION ${${ARGS_NAME}_INSTALL_DIR}/Resources/${helperLib}/${rsc_dir}/${file_dir}
                    )
                endforeach ()
            endif ()
            get_target_property(OriginalLib "${helperLib}" ALIASED_TARGET)
            if (${OriginalLib} STREQUAL "OriginalLib-NOTFOUND")
                #                message(STATUS "The name ${ARGS_HELPER_LIBS} is a REAL target.")
                set(OriginalLib ${helperLib})
            endif ()
            message(STATUS "Installing ${OriginalLib}")
            get_target_property(IsImported "${OriginalLib}" IMPORTED)
            if (${IsImported})
                #                message(STATUS "The name ${OriginalLib} is a IMPORTED target.")
                get_target_property(OriginalLib ${OriginalLib} LOCATION)
                install(FILES ${OriginalLib}
                    DESTINATION ${${ARGS_NAME}_INSTALL_DIR}/lib
                    )
            else ()
                #                message(STATUS "The name ${OriginalLib} is a LOCAL target.")
                install(TARGETS ${OriginalLib}
                    DESTINATION ${${ARGS_NAME}_INSTALL_DIR}
                    LIBRARY DESTINATION ${${ARGS_NAME}_INSTALL_DIR}/lib
                    )
            endif ()
        endforeach ()
    endif ()
    # Configure the resources installation
    if (ARGS_RESOURCES)
        foreach (resLocation ${ARGS_RESOURCES})
            message(STATUS " Installing plugin resources ${resLocation} for ${ARGS_NAME} ")
            installPluginResources(
                TARGET ${ARGS_NAME}
                DIRECTORY ${resLocation}
                BUILD_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/Plugins/Resources/${ARGS_NAME}
                INSTALL_LOCATION ${${ARGS_NAME}_INSTALL_DIR}
            )
        endforeach ()
    endif ()
endfunction()


include(CMakePackageConfigHelpers)
function(configure_radium_library)
    # parse and verify args
    cmake_parse_arguments(
        ARGS
        ""  # no options
        "TARGET;TARGET_DIR;NAMESPACE;PACKAGE_DIR;PACKAGE_CONFIG" # one value args
        "FILES" # list of directories containing the resources to install - optional
        ${ARGN}
    )
    if (NOT ARGS_TARGET)
        message(FATAL_ERROR " [configure_radium_library] You must provide the target to install")
    endif ()
    if (NOT ARGS_TARGET_DIR)
        message(FATAL_ERROR " [configure_radium_library] You must provide the library directory install name")
    endif ()
    if (NOT ARGS_FILES)
        message(FATAL_ERROR " [configure_radium_library] You must provide the library files to install")
    endif ()
    if (NOT ARGS_NAMESPACE)
        set(ARGS_NAMESPACE "Radium")
    endif ()
    if (NOT ARGS_PACKAGE_DIR)
        set(ARGS_PACKAGE_DIR lib/cmake/Radium)
    endif ()

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${ARGS_TARGET}
            PUBLIC
            _DEBUG
            )
        if (MSVC OR MSVC_IDE)
            install(FILES $<TARGET_PDB_FILE:${ARGS_TARGET}> DESTINATION bin)
        endif()
    endif ()
    target_compile_features(${ARGS_TARGET} PUBLIC cxx_std_17)
    if (OPENMP_FOUND)
        target_link_libraries(${ARGS_TARGET} PUBLIC OpenMP::OpenMP_CXX)
    endif (OPENMP_FOUND)

    set(ConfigPackageLocation ${ARGS_PACKAGE_DIR})
    install(TARGETS ${ARGS_TARGET}
        EXPORT ${ARGS_TARGET}Targets
        CONFIGURATIONS ${CMAKE_BUILD_TYPE}
        RUNTIME DESTINATION bin
        OBJECTS DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        INCLUDES DESTINATION include
        )
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${ARGS_TARGET}ConfigVersion.cmake"
        VERSION ${RADIUM_VERSION}
        COMPATIBILITY AnyNewerVersion
    )
    # export for build tree
    export(TARGETS
        ${ARGS_TARGET}
        NAMESPACE
        ${ARGS_NAMESPACE}::
        FILE
        "${CMAKE_CURRENT_BINARY_DIR}/${ARGS_TARGET}Targets.cmake"
        )
    # export for the installation tree
    install(EXPORT ${ARGS_TARGET}Targets
        FILE
        ${ARGS_TARGET}Targets.cmake
        NAMESPACE
        ${ARGS_NAMESPACE}::
        DESTINATION
        ${ConfigPackageLocation}
        )
    if (ARGS_PACKAGE_CONFIG)
        configure_file(${ARGS_PACKAGE_CONFIG}
            "${CMAKE_CURRENT_BINARY_DIR}/${ARGS_TARGET}Config.cmake"
            @ONLY
            )
        install(
            FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${ARGS_TARGET}Config.cmake"
            DESTINATION
            ${ConfigPackageLocation}
        )
    endif ()
    foreach (file ${ARGS_FILES})
        get_filename_component(file_dir ${file} DIRECTORY)
        install(FILES ${file} DESTINATION include/${ARGS_TARGET_DIR}/${file_dir})
    endforeach ()
endfunction()
