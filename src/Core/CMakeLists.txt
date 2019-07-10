set(core_target radiumCore)


set(RADIUM_VERSION_FILE ${CMAKE_CURRENT_BINARY_DIR}/Version.cpp)
add_custom_command(
    OUTPUT ${RADIUM_VERSION_FILE}
    COMMAND ${CMAKE_COMMAND}
                             -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
                             -DCMAKE_CXX_COMPILER_VERSION=${CMAKE_CXX_COMPILER_VERSION}
                             -DCMAKE_CXX_FLAGS=${CMAKE_CXX_FLAGS}
                             -DCMAKE_CXX_FLAGS_DEBUG=${CMAKE_CXX_FLAGS_DEBUG}
                             -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
                             -DGIT_CHANGESET=${GIT_CHANGESET}
                             -P ${CMAKE_CURRENT_SOURCE_DIR}/CMakeGenerateVersion.txt
    COMMENT "Generate Radium Version file"
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/CMakeGenerateVersion.txt
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
    )

file(GLOB_RECURSE core_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE core_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE core_inlines ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

set(core_libs
    ${OPENMESH_LIBRARIES}
   )

add_library(
    ${core_target} SHARED
    ${core_sources}
    ${core_headers}
    ${core_inlines}
    ${RADIUM_VERSION_FILE} #This file is build-dependant and generated in the Bundle
)

add_dependencies( ${core_target} eigen3  openmesh_lib)

# Define the base path for default shader. To be defined according to the install operation.
message("Setting radium env base dir to ${RADIUM_BINARY_OUTPUT_PATH}")

target_compile_definitions(${core_target} PRIVATE "-DRA_CORE_EXPORTS" RADIUM_BIN_BASEDIR="${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/")
target_include_directories(${core_target}
    PRIVATE
           ${CMAKE_CURRENT_SOURCE_DIR}/..
	PUBLIC
           ${EIGEN3_INCLUDE_DIR}
           ${OPENMESH_INCLUDE_DIR}
    )

target_link_libraries(
    ${core_target}
    PUBLIC ${core_libs}
)
radium_cotire(${core_target})

set_property( TARGET ${core_target} PROPERTY POSITION_INDEPENDENT_CODE ON )
