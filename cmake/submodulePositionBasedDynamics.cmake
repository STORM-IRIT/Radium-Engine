set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake" ${CMAKE_MODULE_PATH})
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# here is defined the way we want to import PBD
ExternalProject_Add(
        PositionBasedDynamics
        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/PositionBasedDynamics"

        # override default behaviours
        UPDATE_COMMAND ""
        GIT_SUBMODULES 3rdPartyLibraries/PositionBasedDynamics

        # set the installatin to installed/PBD
        INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}"
        BUILD_BYPRODUCTS "${POSITIONBASEDDYNAMICS_LIBRARIES}"
        CMAKE_GENERATOR ${CMAKE_GENERATOR}
        CMAKE_GENERATOR_TOOLSET ${CMAKE_GENERATOR_TOOLSET}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DPOSITIONBASEDDYNAMICS_BUILD_POSITIONBASEDDYNAMICS_TOOLS=False
            -DPOSITIONBASEDDYNAMICS_BUILD_SAMPLES=False
            -DPOSITIONBASEDDYNAMICS_BUILD_TESTS=False
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_CXX_STANDARD=${CMAKE_CXX_STANDARD}
            -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
            -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
            -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
            -DCMAKE_CXX_FLAGS_RELWITHDEBINFO=${CMAKE_CXX_FLAGS_RELWITHDEBINFO}
            -DCMAKE_SHARED_LINKER_FLAGS=${CMAKE_SHARED_LINKER_FLAGS}
            -DCMAKE_BUILD_TYPE=${RADIUM_SUBMODULES_BUILD_TYPE}
            -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
            -DPOSITIONBASEDDYNAMICS_INSTALL_PDB=False
            -DINSTALL_NAME_DIR=${CMAKE_RUNTIME_OUTPUT_DIRECTORY} #override rpath to solve run bug on MACOSX
)


## GenericParameters
ExternalProject_Add(
   Ext_GenericParameters
   PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/3rdPartyLibraries/PositionBasedDynamics/extern/GenericParameters"
   GIT_REPOSITORY https://github.com/InteractiveComputerGraphics/GenericParameters.git
   GIT_TAG "1ec904bf8555e78ae0d8ba2f9f9a395371c5d4eb"
   INSTALL_DIR "${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/GenericParameters"
   CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX:PATH=${RADIUM_SUBMODULES_INSTALL_DIRECTORY}/include/GenericParameters -DGENERICPARAMETERS_NO_TESTS:BOOL=1
)

if( MSVC )

    add_custom_target(PositionBasedDynamics_prerename_compiled_lib_and_dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${POSITIONBASEDDYNAMICS_LIBRARIES_COMPATNAME}" "${POSITIONBASEDDYNAMICS_LIBRARIES}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${POSITIONBASEDDYNAMICS_DLL_COMPATNAME}" "${POSITIONBASEDDYNAMICS_DLL}"
        COMMENT "copy PBD libs and dll using the generic filename used on other OS. This is messy" VERBATIM
        DEPENDS PositionBasedDynamics
    )

    add_custom_target( PositionBasedDynamics_install_compiled_dll
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POSITIONBASEDDYNAMICS_COMPATNAME} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POSITIONBASEDDYNAMICS_DLL} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        COMMENT "copy PBD dll to bin dir" VERBATIM
        DEPENDS PositionBasedDynamics_prerename_compiled_lib_and_dll
    )

endif()

if( MINGW )
    add_custom_target( PositionBasedDynamics_install_compiled_dll
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${POSITIONBASEDDYNAMICS_DLL} "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
            COMMENT "copy PositionBasedDynamics dll to bin dir" VERBATIM
            DEPENDS PositionBasedDynamics create_bin_dir
    )

endif()
