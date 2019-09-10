# installLibHeaders "Core" "Utils/foo.h" -> install in include/Core/Utils/foo.h
# You must pass both parameters using quotes
macro( installLibHeaders TARGET_DIR_NAME FILES )
    foreach ( file ${FILES} )
        get_filename_component( file_dir ${file} DIRECTORY )
        install( FILES ${file} DESTINATION include/${TARGET_DIR_NAME}/${file_dir} )
    endforeach()
endmacro()
