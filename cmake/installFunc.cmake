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
    foreach ( file ${FILES} )
        get_filename_component( file_dir ${file} DIRECTORY )
        install( FILES ${RADIUM_SHADERS_DIR}/${file} DESTINATION bin/Shaders/${file_dir} )
#        file( COPY ${RADIUM_SHADERS_DIR}/${file} DESTINATION ../../tests/ExampleApps/Shaders/${file_dir})
    endforeach()
endmacro()
