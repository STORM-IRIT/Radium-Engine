# Helper macro for creating convenient targets

# ------------------------------------------------------------------------------
# Usefull for adding header only libraries
# ~~~
# Example usage:
#     external_header_only_add("Catch"
#         "https://github.com/catchorg/Catch2.git" "origin/master" "single_include/catch2")
# ~~~
# Use with target_link_libraries(unittests Catch) This will add the INCLUDE_FOLDER_PATH to the
# `unittests` target.

macro(external_header_only_add LIBNAME REPOSITORY GIT_TAG INCLUDE_FOLDER_PATH)
    ExternalProject_Add(
        ${LIBNAME}_download
        PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}
        GIT_REPOSITORY ${REPOSITORY}
        # For shallow git clone (without downloading whole history) GIT_SHALLOW 1 For point at
        # certain tag
        GIT_TAG ${GIT_TAG}
        # disables auto update on every build
        UPDATE_DISCONNECTED 1
        # disable following
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_DIR ""
        INSTALL_COMMAND ""
    )
    # special target
    add_custom_target(
        ${LIBNAME}_update
        COMMENT "Updated ${LIBNAME}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}/src/${LIBNAME}_download
        COMMAND ${GIT_EXECUTABLE} fetch --recurse-submodules
        COMMAND ${GIT_EXECUTABLE} reset --hard ${GIT_TAG}
        COMMAND ${GIT_EXECUTABLE} submodule update --init --force --recursive --remote --merge
        DEPENDS ${LIBNAME}_download
    )

    set(${LIBNAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}/src/${LIBNAME}_download/)
    add_library(${LIBNAME} INTERFACE)
    add_dependencies(${LIBNAME} ${LIBNAME}_download)
    add_dependencies(update ${LIBNAME}_update)
    target_include_directories(
        ${LIBNAME} SYSTEM
        INTERFACE
            ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}/src/${LIBNAME}_download/${INCLUDE_FOLDER_PATH}
    )
endmacro()

# ------------------------------------------------------------------------------
# This command will clone git repo during cmake setup phase, also adds ${LIBNAME}_update target into
# general update target.
# ~~~
# Example usage:
#   external_download_now_git(cpr https://github.com/finkandreas/cpr.git origin/master)
#   add_subdirectory(${cpr_SOURCE_DIR})
# ~~~

macro(external_download_now_git LIBNAME REPOSITORY GIT_TAG)

    set(${LIBNAME}_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}/src/${LIBNAME}_download/)

    # clone repository if not done
    if(IS_DIRECTORY ${${LIBNAME}_SOURCE_DIR})
        message(STATUS "Already downloaded: ${REPOSITORY}")
    else()
        message(STATUS "Clonning: ${REPOSITORY}")
        execute_process(
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
            COMMAND ${GIT_EXECUTABLE} clone --recursive ${REPOSITORY}
                    ${LIBNAME}/src/${LIBNAME}_download
        )
        # switch to target TAG and update submodules
        execute_process(
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}/src/${LIBNAME}_download
            COMMAND ${GIT_EXECUTABLE} reset --hard ${GIT_TAG}
            COMMAND ${GIT_EXECUTABLE} submodule update --init --force --recursive --remote --merge
        )
    endif()

    # special update target
    add_custom_target(
        ${LIBNAME}_update
        COMMENT "Updated ${LIBNAME}"
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/${LIBNAME}/src/${LIBNAME}_download
        COMMAND ${GIT_EXECUTABLE} fetch --recurse-submodules
        COMMAND ${GIT_EXECUTABLE} reset --hard ${GIT_TAG}
        COMMAND ${GIT_EXECUTABLE} submodule update --init --force --recursive --remote --merge
    )
    # Add this as dependency to the general update target
    add_dependencies(update ${LIBNAME}_update)
endmacro()

# ------------------------------------------------------------------------------
# Force compilers, this was deprecated in CMake, but still comes handy sometimes
macro(FORCE_C_COMPILER compiler id)
    set(CMAKE_C_COMPILER "${compiler}")
    set(CMAKE_C_COMPILER_ID_RUN TRUE)
    set(CMAKE_C_COMPILER_ID ${id})
    set(CMAKE_C_COMPILER_FORCED TRUE)

    # Set old compiler id variables.
    if(CMAKE_C_COMPILER_ID MATCHES "GNU")
        set(CMAKE_COMPILER_IS_GNUCC 1)
    endif()
endmacro()

macro(FORCE_CXX_COMPILER compiler id)
    set(CMAKE_CXX_COMPILER "${compiler}")
    set(CMAKE_CXX_COMPILER_ID_RUN TRUE)
    set(CMAKE_CXX_COMPILER_ID ${id})
    set(CMAKE_CXX_COMPILER_FORCED TRUE)

    # Set old compiler id variables.
    if("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
        set(CMAKE_COMPILER_IS_GNUCXX 1)
    endif()
endmacro()
