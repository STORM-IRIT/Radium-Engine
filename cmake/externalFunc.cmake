macro(addExternalFolder NAME FOLDER )
    # External resources/repositories are downloaded and built at configuration stage
    message(INFO "[addExternalFolder] process ${NAME} ${FOLDER}")

    message(STATUS "[addExternalFolder] Create temporary directory")
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/external/cmake
    )

    message(STATUS "[addExternalFolder] Run cmake with options ${ARGN}")
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
            -DCMAKE_BUILD_PARALLEL_LEVEL=${CMAKE_BUILD_PARALLEL_LEVEL}
            ${ARGN}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external
    )
    message(STATUS "[addExternalFolder] Start build")

    set( RadiumExternalMakeTarget all)
    string( TOLOWER ${CMAKE_GENERATOR} generator_lower)
    if( MSVC AND NOT ${generator_lower} STREQUAL "ninja" )
            set( RadiumExternalMakeTarget ALL_BUILD)
            message(INFO "[addExternalFolder] Enable compatibility mode for VS Generator" )
    endif()

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE} --target ${RadiumExternalMakeTarget}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external
    )
    message(STATUS "[addExternalFolder] Build ended")

    #Configure package
    set(external_sources ${FOLDER}/CMakeLists.txt)
    if(EXISTS "${FOLDER}/package.cmake")
        set(EXTERNAL_TARGET_PACKAGE_FILE "${CMAKE_CURRENT_BINARY_DIR}/external/cmake/package-${NAME}.cmake")
        message(STATUS "[addExternalFolder] Configure package file ${EXTERNAL_TARGET_PACKAGE_FILE}")
        configure_file(${FOLDER}/package.cmake "${EXTERNAL_TARGET_PACKAGE_FILE}" COPYONLY)
        unset(EXTERNAL_TARGET_PACKAGE_FILE)
        include("${CMAKE_CURRENT_BINARY_DIR}/external/cmake/package-${NAME}.cmake")
        list(APPEND external_sources ${FOLDER}/package.cmake)
    endif()

    #This dummy target add external/CMakeLists.txt to most IDEs
    add_custom_target(External${NAME} ALL SOURCES ${external_sources})
endmacro()
