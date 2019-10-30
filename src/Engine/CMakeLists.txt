set(ra_engine_target RadiumEngine)

project( ${ra_engine_target}
         LANGUAGES CXX
         VERSION ${RADIUM_VERSION})

include( filelistEngine )
set(engine_shaders_full ${engine_shaders})

if(CMAKE_VERSION VERSION_LESS 3.12)
    string(REGEX REPLACE "([^;]+)" "${RADIUM_SHADERS_DIR}/\\1" engine_shaders_full "${engine_shaders}")
else()
    list(TRANSFORM engine_shaders_full PREPEND ${RADIUM_SHADERS_DIR}/)
endif()

add_library(
    ${ra_engine_target} SHARED
    ${engine_sources}
    ${engine_headers}
    ${engine_inlines}
)
add_library(Radium::Engine ALIAS ${ra_engine_target})

add_custom_target(RadiumEngineShaders ALL SOURCES ${engine_shaders_full})

addExternalFolder( Engine ${CMAKE_CURRENT_SOURCE_DIR}/external )

find_package(glbinding REQUIRED NO_DEFAULT_PATH )
find_package(globjects REQUIRED NO_DEFAULT_PATH )

configureTargetWithDefaultRaSettings( ${ra_engine_target} )

target_include_directories(${ra_engine_target} PUBLIC
    $<BUILD_INTERFACE:${GLBINDING_INCLUDE_DIR}>
    $<BUILD_INTERFACE:${GLM_INCLUDE_DIR}> )
add_dependencies (${ra_engine_target} Radium::Core RadiumEngineShaders)
target_link_libraries (${ra_engine_target} PUBLIC Radium::Core glm glbinding::glbinding glbinding::glbinding-aux globjects::globjects)

target_compile_definitions(${ra_engine_target} PRIVATE "-DRA_ENGINE_EXPORTS")

# set_target_properties(${ra_engine_target} PROPERTIES PUBLIC_HEADER "${engine_headers};${engine_inlines}")

installLibHeaders( "Engine" "${engine_headers}" )
installLibHeaders( "Engine" "${engine_inlines}" )

message(STATUS "[Engine] installing resources from ${RADIUM_SHADERS_DIR}")
installResources(TARGET ${ra_engine_target} DIRECTORY ${RADIUM_SHADERS_DIR} FILES ${engine_shaders})
