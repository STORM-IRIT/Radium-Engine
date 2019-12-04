set( CMAKE_EXECUTE_PROCESS_COMMAND_ECHO STDOUT)

macro(addExternalFolder NAME FOLDER )
    # External resources/repositories are downloaded and built at configuration stage
    message(INFO "[addExternalFolder] process ${NAME} ${FOLDER}")

    message(STATUS "[addExternalFolder] Create temporary directory")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/external/cmake
    )

    if("${ARGN}" STREQUAL "")
        message(STATUS "[addExternalFolder] Configure cmake project")
    else ()
        message(STATUS "[addExternalFolder] Configure cmake project with options ${ARGN}")
    endif()

    string(TOUPPER ${NAME} NAME_UPPER)
    set( UPDATE_EXTERNAL ON)
    if( RADIUM_SKIP_${NAME_UPPER}_EXTERNAL )
        set( UPDATE_EXTERNAL OFF)
    endif()


    if ( UPDATE_EXTERNAL )
        execute_process(
            COMMAND ${CMAKE_COMMAND} ${FOLDER}
                -DCMAKE_GENERATOR=${CMAKE_GENERATOR}
                -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
                -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
                -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
                -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
                -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
                -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
                -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
                -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
                -DCMAKE_MODULE_PATH=${CMAKE_MODULE_PATH}
                -DCMAKE_MACOSX_RPATH=TRUE
                ${ARGN}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external
            RESULT_VARIABLE ret
        )
        if(NOT ret EQUAL "0")
            message( FATAL_ERROR "[addExternalFolder] Cmake configure step failed. ")
        endif()
        message(STATUS "[addExternalFolder] Start build")

        set( RadiumExternalMakeTarget all)
        string( TOLOWER ${CMAKE_GENERATOR} generator_lower)
        if( MSVC AND NOT ${generator_lower} STREQUAL "ninja" )
                set( RadiumExternalMakeTarget ALL_BUILD)
                message(INFO "[addExternalFolder] Enable compatibility mode for VS Generator" )
        endif()

        if( ${CMAKE_VERSION} VERSION_GREATER_EQUAL 3.12 )
        execute_process(
            COMMAND ${CMAKE_COMMAND} --build . -j ${RADIUM_BUILD_EXTERNAL_PARALLEL_LEVEL} --config ${CMAKE_BUILD_TYPE} --target ${RadiumExternalMakeTarget}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external
            RESULT_VARIABLE ret
            )
        else()
            set( TOOL_OPTIONS "" )
            if( ${generator_lower} MATCHES "|makefile|" OR ${generator_lower} MATCHES "|ninja|" )
              set( TOOL_OPTIONS -- -j${RADIUM_BUILD_EXTERNAL_PARALLEL_LEVEL} )
            elseif( ${generator_lower} MATCHES "|visual|" )
              set( TOOL_OPTIONS -- /m )
            else()
              message(WARNING "[addExternalFolder] Parallel build are not supported with your generator ${CMAKE_GENERATOR}")
            endif()
            execute_process(
              COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --target ${RadiumExternalMakeTarget} ${TOOL_OPTIONS}
              WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external
              RESULT_VARIABLE ret
              )
        endif()

        if(NOT ret EQUAL "0")
            message( FATAL_ERROR "[addExternalFolder] Cmake build step failed. ")
        endif()
        message(STATUS "[addExternalFolder] Build ended")

        #Configure package
        if(EXISTS "${FOLDER}/package.cmake")
            set(EXTERNAL_TARGET_PACKAGE_FILE "${CMAKE_CURRENT_BINARY_DIR}/external/cmake/package-${NAME}.cmake")
            message(STATUS "[addExternalFolder] Configure package file ${EXTERNAL_TARGET_PACKAGE_FILE}")
            configure_file(${FOLDER}/package.cmake "${EXTERNAL_TARGET_PACKAGE_FILE}" COPYONLY)
            unset(EXTERNAL_TARGET_PACKAGE_FILE)
            include("${CMAKE_CURRENT_BINARY_DIR}/external/cmake/package-${NAME}.cmake")
        endif()
        OPTION( RADIUM_SKIP_${NAME_UPPER}_EXTERNAL "[addExternalFolder] Skip updating ${NAME}::external (disable for rebuild)" ON)

    else()
        message("[addExternalFolder] Skip ${NAME}::external update (use RADIUM_SKIP_${NAME_UPPER}_EXTERNAL to change behavior)")
    endif()

    #This dummy target add external/CMakeLists.txt to most IDEs
    set(external_sources ${FOLDER}/CMakeLists.txt)
    if(EXISTS "${FOLDER}/package.cmake")
        list(APPEND external_sources ${FOLDER}/package.cmake)
    endif()
    add_custom_target(External${NAME} ALL SOURCES ${external_sources})
    include("${CMAKE_CURRENT_BINARY_DIR}/external/cmake/package-${NAME}.cmake")

endmacro()
