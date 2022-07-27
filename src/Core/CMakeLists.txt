set(RA_CORE_TARGET Core)

project(${RA_CORE_TARGET} LANGUAGES CXX VERSION ${RADIUM_VERSION})

option(RADIUM_QUIET "Disable Radium Log messages" OFF)
list(APPEND CMAKE_MESSAGE_INDENT "[${RA_CORE_TARGET}] ")

set(RA_VERSION_CPP "${CMAKE_CURRENT_BINARY_DIR}/Version.cpp")
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/Utils/Version.cpp.in" "${RA_VERSION_CPP}")
# If you want to have date and time of the build, your targets has to depend on this. This will
# force recompilation of version.o and thus forcing gcc to update __DATE__ macro.
if(RADIUM_UPDATE_VERSION)
    add_custom_target(
        versionFileTouchForRebuild COMMAND ${CMAKE_COMMAND} -E touch "${RA_VERSION_CPP}"
        COMMENT "Update Version file"
    )
else()
    add_custom_target(versionFileTouchForRebuild COMMENT "Dummy update Version file.")
endif()

find_package(Threads REQUIRED)
find_package(Filesystem COMPONENTS Final Experimental REQUIRED)
find_package(Backtrace)

include(filelist.cmake)

add_library(
    ${RA_CORE_TARGET} SHARED ${core_sources} ${core_headers} ${core_inlines} ${RA_VERSION_CPP}
)

populate_local_dependencies(NAME "Eigen3_DIR")
populate_local_dependencies(NAME "OpenMesh_DIR")
populate_local_dependencies(NAME "cpplocate_DIR")
populate_local_dependencies(NAME "nlohmann_json_DIR")

find_package(Eigen3 3.3 REQUIRED NO_DEFAULT_PATH)

find_package(OpenMesh REQUIRED COMPONENTS Core Tools NO_DEFAULT_PATH)

find_package(cpplocate REQUIRED NO_DEFAULT_PATH)

find_package(nlohmann_json REQUIRED NO_DEFAULT_PATH)

add_dependencies(${RA_CORE_TARGET} versionFileTouchForRebuild) # We want precise time of build in
                                                               # version

target_compile_options(${RA_CORE_TARGET} PUBLIC ${RA_DEFAULT_COMPILE_OPTIONS})

target_include_directories(
    ${RA_CORE_TARGET} PUBLIC $<BUILD_INTERFACE:${OPENMESH_INCLUDE_DIR}>
                             $<BUILD_INTERFACE:${EIGEN_INCLUDE_DIR}>
)
target_link_libraries(
    ${RA_CORE_TARGET} PUBLIC OpenMeshCore OpenMeshTools Threads::Threads Eigen3::Eigen
                             cpplocate::cpplocate nlohmann_json::nlohmann_json std::filesystem
)

target_compile_definitions(${RA_CORE_TARGET} PRIVATE RA_CORE_EXPORTS)
target_compile_definitions(
    ${RA_CORE_TARGET}
    PUBLIC -DCXX_FILESYSTEM_HAVE_FS
           -DCXX_FILESYSTEM_IS_EXPERIMENTAL=$<BOOL:${CXX_FILESYSTEM_IS_EXPERIMENTAL}>
           -DCXX_FILESYSTEM_NAMESPACE=${CXX_FILESYSTEM_NAMESPACE}
)

configure_file(radium_backtrace.h.in radium_backtrace.h)
target_include_directories(${RA_CORE_TARGET} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
if(Backtrace_FOUND)
    target_include_directories(${RA_CORE_TARGET} PRIVATE ${Backtrace_INCLUDE_DIRS})
    if(DEFINED Backtrace_LIBRARIES)
        target_link_libraries(${RA_CORE_TARGET} PRIVATE ${Backtrace_LIBRARIES})
    endif()
endif()

if(WIN_32)
    target_compile_definitions(${RA_CORE_TARGET} PUBLIC _USE_MATH_DEFINES) # OpenMesh
endif()
if(${RADIUM_QUIET})
    target_compile_definitions(${RA_CORE_TARGET} PUBLIC RA_NO_LOG)
    message(STATUS "${PROJECT_NAME} : Radium Logs disabled")
endif()

message(STATUS "Configuring library ${RA_CORE_TARGET} with standard settings")
configure_radium_target(${RA_CORE_TARGET})
configure_radium_library(
    TARGET ${RA_CORE_TARGET} COMPONENT PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
    FILES "${core_headers};${core_inlines}"
)
set(RADIUM_COMPONENTS ${RADIUM_COMPONENTS} ${RA_CORE_TARGET} PARENT_SCOPE)

if(RADIUM_ENABLE_PCH)
    target_precompile_headers(${RA_CORE_TARGET} PRIVATE pch.hpp)
endif()

list(REMOVE_AT CMAKE_MESSAGE_INDENT -1)
