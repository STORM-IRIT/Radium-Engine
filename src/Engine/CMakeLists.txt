set(RA_ENGINE_TARGET Engine)
list(APPEND CMAKE_MESSAGE_INDENT "[${RA_ENGINE_TARGET}] ")

project(${RA_ENGINE_TARGET} LANGUAGES CXX VERSION ${RADIUM_VERSION})

include(filelist.cmake)
set(ENGINE_SHADERS_FULL ${ENGINE_SHADERS})

if(CMAKE_VERSION VERSION_LESS 3.16)
    string(REGEX REPLACE "([^;]+)" "${RADIUM_SHADERS_DIR}/\\1" ENGINE_SHADERS_FULL
                         "${ENGINE_SHADERS}"
    )
else()
    list(TRANSFORM ENGINE_SHADERS_FULL PREPEND ${RADIUM_SHADERS_DIR}/)
endif()

add_library(${RA_ENGINE_TARGET} SHARED ${ENGINE_SOURCES} ${ENGINE_HEADERS} ${ENGINE_INLINES})

add_custom_target(
    RadiumEngineShaders ALL SOURCES ${ENGINE_SHADERS_FULL} COMMENT "Shaders source list."
)

# LOCAL_DEPENDENCIES form parent scope
populate_local_dependencies(NAME "glm_DIR")
populate_local_dependencies(NAME "glbinding_DIR")
populate_local_dependencies(NAME "globjects_DIR")
populate_local_dependencies(NAME "stb_DIR")
populate_local_dependencies(NAME "tinyEXR_DIR")

find_package(glbinding REQUIRED NO_DEFAULT_PATH)
find_package(globjects REQUIRED NO_DEFAULT_PATH)
find_package(Eigen3 REQUIRED NO_DEFAULT_PATH)
find_package(tinyEXR REQUIRED NO_DEFAULT_PATH)

# This one should be extracted directly from external project properties.
target_compile_definitions(${RA_ENGINE_TARGET} PUBLIC GLOBJECTS_USE_EIGEN)

target_compile_options(${RA_ENGINE_TARGET} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})

target_include_directories(
    ${RA_ENGINE_TARGET} PUBLIC $<BUILD_INTERFACE:${GLBINDING_INCLUDE_DIR}>
                               $<BUILD_INTERFACE:${GLM_INCLUDE_DIR}>
)
add_dependencies(${RA_ENGINE_TARGET} Core RadiumEngineShaders)
target_link_libraries(
    ${RA_ENGINE_TARGET} PUBLIC Core glm glbinding::glbinding glbinding::glbinding-aux
                               globjects::globjects tinyEXR::tinyEXR
)

target_compile_definitions(${RA_ENGINE_TARGET} PRIVATE "-DRA_ENGINE_EXPORTS")

message(STATUS "Configuring library ${RA_ENGINE_TARGET} with standard settings")
configure_radium_target(${RA_ENGINE_TARGET})
configure_radium_library(
    TARGET ${RA_ENGINE_TARGET} COMPONENT PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    FILES "${ENGINE_HEADERS};${ENGINE_INLINES}"
)
set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${RA_ENGINE_TARGET} PARENT_SCOPE)

message(STATUS "installing resources from ${RADIUM_SHADERS_DIR}")
install_target_resources(
    TARGET ${RA_ENGINE_TARGET} RESOURCES_DIR ${RADIUM_SHADERS_DIR} RESOURCES_INSTALL_DIR "Radium"
    FILES ${ENGINE_SHADERS}
)
message(STATUS "installing resources from ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Textures")
install_target_resources(
    TARGET ${RA_ENGINE_TARGET} RESOURCES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Resources/Textures
    RESOURCES_INSTALL_DIR "Radium"
)

target_compile_options(${RA_ENGINE_TARGET} PRIVATE PUBLIC ${DEFAULT_COMPILE_OPTIONS} INTERFACE)

if(RADIUM_ENABLE_PCH)
    target_precompile_headers(${RA_ENGINE_TARGET} PRIVATE pch.hpp)
endif()

list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
