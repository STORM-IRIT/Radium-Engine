# installLibHeaders "Core" "Utils/foo.h" -> install in include/Core/Utils/foo.h
# You must pass both parameters using quotes
macro( installLibHeaders TARGET_DIR_NAME FILES )
    foreach ( file ${FILES} )
        get_filename_component( file_dir ${file} DIRECTORY )
        install( FILES ${file} DESTINATION include/${TARGET_DIR_NAME}/${file_dir} )
    endforeach()
endmacro()

# Copy shaders to buid and install directories
# installLibShaders "foo.glsl"
#      -> copy to build/tests/ExampleApps/Shaders
#      -> install in bin/Shaders/foo.glsl
# You must pass parameter using quotes
macro( installLibShaders FILES )
    # build configuration : link shaders to Resources/Shaders
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/../Resources")
    file(CREATE_LINK ${RADIUM_SHADERS_DIR} "${CMAKE_CURRENT_BINARY_DIR}/../Resources/Shaders" COPY_ON_ERROR SYMBOLIC)
    foreach ( file ${FILES} )
        get_filename_component( file_dir ${file} DIRECTORY )
        # installed library
        install(FILES ${RADIUM_SHADERS_DIR}/${file} DESTINATION Resources/Shaders/${file_dir})
    endforeach()
endmacro()
