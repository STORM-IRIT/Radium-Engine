cmake_minimum_required(VERSION 3.13)

#------------------------------------------------------------------------------
# Policies and global parameters for CMake
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

# Enable helpfull warnings and C++17 for all files
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Project setup, versioning stuff here, change when changing the version
# Note: keep the project name lower case only for easy linux packaging support
project (radiumproject VERSION 1.0.0)
set(VERSION_TYPE "beta" CACHE STRING "version type" FORCE)
site_name(VERSION_HOST) # read hostname to VERSION_HOST
set(VERSION_HOST "${VERSION_HOST}" CACHE STRING "host of build" FORCE)

set(RADIUM_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set(RADIUM_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})

option(RADIUM_ENABLE_TESTING          "Enable testing. Tests are automatically built with target all, run with target check or test" ON)
option(RADIUM_ENABLE_COVERAGE         "Enable coverage, gcc only. Experimental, need ENABLE_TESTING" OFF)
option(RADIUM_GENERATE_LIB_CORE       "Include Radium::Core in CMake project" ON)
option(RADIUM_GENERATE_LIB_IO         "Include Radium::IO in CMake project" ON)
option(RADIUM_GENERATE_LIB_ENGINE     "Include Radium::Engine in CMake project" ON)
option(RADIUM_GENERATE_LIB_GUIBASE    "Include Radium::GuiBase in CMake project" ON)
option(RADIUM_GENERATE_LIB_PLUGINBASE "Include Radium::PluginBase in CMake project" ON)
option(RADIUM_UPDATE_VERSION          "Update version file each time the project is compiled (update compilation time in version.cpp)" ON)
option(RADIUM_INSTALL_DOC             "Install documentation. If RadiumDoc is compiled, install documentation to bundle directory for install target" ON)

set(RADIUM_EXTERNAL_CMAKE_INSTALL_MESSAGE NEVER)

set(RADIUM_VERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH})
set(RADIUM_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})

set(DISPLAY_WIDTH 80)

function(messageFormat TEXT PRE PRE_LEN POST POST_LEN FILL)
    set(TEXT_LEN 0)
    if(TEXT)
        string(LENGTH ${TEXT} TEXT_LEN)
    endif()
    MATH(EXPR LEN "${DISPLAY_WIDTH} - ${TEXT_LEN} - ${PRE_LEN} - ${POST_LEN}")
    set(FILL1 "")
    if(LEN LESS 0)
        MATH(EXPR SUBLEN "${DISPLAY_WIDTH} - ${PRE_LEN} - ${POST_LEN}-1")
        string(SUBSTRING ${TEXT} 0  ${SUBLEN} STRING1)
        MATH(EXPR PL2 "${POST_LEN}+1")
        messageFormat(${STRING1} ${PRE} ${PRE_LEN} "⏎${POST}" ${PL2} ${FILL})
        string(SUBSTRING ${TEXT} ${SUBLEN} -1 STRING2)
        messageFormat(${STRING2} ${PRE} ${PRE_LEN} ${POST} ${POST_LEN} ${FILL})
    else()
        foreach (i RANGE ${LEN})
            set(FILL1 "${FILL1}${FILL}")
        endforeach()
    
        message(NOTICE "${PRE}${TEXT}${FILL1}${POST}")
    endif()
endfunction()

function(messageInfo INFO)  
    set(PRE "│ ")
    set(PRE_LEN 2)
    set(POST "│")
    set(POST_LEN 1)
    messageFormat(${INFO} ${PRE} ${PRE_LEN} ${POST} ${POST_LEN} " ")
endfunction()

function(messageSetting OPTION)
    set(PRE "│ ")
    set(PRE_LEN 2)
    set(POST "│")
    set(POST_LEN 1)

    string(LENGTH ${OPTION} len)
    MATH(EXPR len "30-${len}")
    set(FILL "")
    foreach (i RANGE ${len})
        set(FILL "${FILL} ")
    endforeach()

    if(DEFINED ${OPTION})
        set(MSG "${OPTION}${FILL}${${OPTION}}")
    else()
        set(MSG "${OPTION}${FILL}[undef]")
    endif()
    messageFormat(${MSG} ${PRE} ${PRE_LEN} ${POST} ${POST_LEN} " ")
endfunction()

function(messageTitle TITLE)
    string(LENGTH ${TITLE} len)
    MATH(EXPR len "${DISPLAY_WIDTH}-6-${len}")
    MATH(EXPR len1 "${len}/2")
    MATH(EXPR len1 "${len1}-1")
    MATH(EXPR len2 "${len}-${len1}-1")
    set(FILL1 "")
    foreach (i RANGE ${len1})
        set(FILL1 "${FILL1}─")
    endforeach()
    set(FILL2 "")
    
    foreach (i RANGE ${len2})
        set(FILL2 "${FILL2}─")
    endforeach()
    message(NOTICE "┌${FILL1}┤│${TITLE}│├${FILL2}┐")
endfunction()

function(messageEnd)
    set(PRE "└")
    set(PRE_LEN 1)
    set(POST "┘")
    set(POST_LEN 1)
    messageFormat("" ${PRE} ${PRE_LEN} ${POST} ${POST_LEN} "─")
endfunction()


# Changing the default value for CMAKE_BUILD_PARALLEL_LEVEL
if(NOT DEFINED ENV{CMAKE_BUILD_PARALLEL_LEVEL})
    include(ProcessorCount)
    ProcessorCount(N)
    if(NOT N EQUAL 0)
        set(CTEST_BUILD_FLAGS -j${N})
        set(ctest_test_args ${ctest_test_args} PARALLEL_LEVEL ${N})
        set(ENV{CMAKE_BUILD_PARALLEL_LEVEL} ${N})
    endif()
endif()

messageTitle(" ${PROJECT_NAME} Project configuration ")
messageInfo("Externals will be built with $ENV{CMAKE_BUILD_PARALLEL_LEVEL} core(s)")

#------------------------------------------------------------------------------
# General settings


if(NOT CMAKE_BUILD_TYPE)
  messageInfo("No CMAKE_BUILD_TYPE, using default CMAKE_BUILD_TYPE=Release")
  set(CMAKE_BUILD_TYPE "Release")
endif()

# Be nice to visual studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Be nice and export compile commands by default, this is handy for clang-tidy
# and for other tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We can use include() and find_package() for our scripts in there
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Configure rpath, see cmake/rpath.cmake
include(rpath)

# Use gold linker to speed up linking time, see cmake/useGoldLinker.cmake
include(useGoldLinker)

# Helpful option enable build profiling to identify slowly compiling files
option(MEASURE_ALL "When enabled all commands will be passed through time command" OFF)
if(MEASURE_ALL)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "time")
endif()

# Append library and executable names with d in debug mode
set(CMAKE_DEBUG_POSTFIX d)

#-------------------------------------------------------------------------------
# Set default paths for Radium
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID})
else ()
    set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE})
endif ()
set(RADIUM_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(RADIUM_SHADERS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Shaders")

#-------------------------------------------------------------------------------
# Set default install location to RADIUM_BUNDLE_DIRECTORY
# we do not want to install to /usr by default
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${RADIUM_BUNDLE_DIRECTORY}" CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE)
    messageInfo("Set install prefix to ${CMAKE_INSTALL_PREFIX}")
endif()

#------------------------------------------------------------------------------
# Custom Install target, used in run target in source/CMakeLists.txt
add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target
    install WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")

#------------------------------------------------------------------------------
# get changeset id
find_package(Git QUIET)

if(GIT_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git")
    option(RADIUM_GIT_UPDATE_SUBMODULE         "Check submodules during build (will be automatically disabled after run)" ON)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_CHANGESET)
    if ( GIT_CHANGESET )
      # remove new line sometime appearing in git changeset
      string(REGEX REPLACE "\n$" "" GIT_CHANGESET "${GIT_CHANGESET}")
    endif()
else()
  set(GIT_CHANGESET "")
endif()
messageInfo("Git Changeset: ${GIT_CHANGESET}")

messageEnd()
set(ENABLE_COVERAGE "")
if(RADIUM_ENABLE_COVERAGE)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
        message( STATUS "[Tests] Enable Coverage ¡Warning! slow down execution ¡Warning!")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -Wall -W -Wshadow -Wunused-variable -Wunused-parameter -Wunused-function -Wunused -Wno-system-headers -Wno-deprecated -Woverloaded-virtual -Wwrite-strings -fprofile-arcs -ftest-coverage")
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -O0 -Wall -W -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}-fprofile-arcs -ftest-coverage")
        find_program(LCOV_BIN lcov)

        if(NOT LCOV_BIN)
            message( FATAL_ERROR "lcov binary not found, while asking for coverage computation (RADIUM_ENABLE_COVERAGE=${RADIUM_ENABLE_COVERAGE}). Abort")
        endif()
        set(RADIUM_ENABLE_TESTING "ON")
        set(ENABLE_COVERAGE "ON")
    else()
        message( WARNING "[Tests] Coverage requested (RADIUM_ENABLE_COVERAGE), but not activated.")
        message( WARNING "[Tests] Coverage only supported in DEBUG with GCC.")
    endif()
endif(RADIUM_ENABLE_COVERAGE)

#------------------------------------------------------------------------------
# Dependency management utilities
include(externalFunc)
# Installation utilities
include(radium_setup_functions)

# Documentation build
add_subdirectory(doc)


# This var gather local dependencies in subdirectories
set(LocalDependencies)

# Images, databases and other data which needs to be installed for project
#add_subdirectory(data)

# Source code
add_subdirectory(src)

# Add this target so that some IDE could propose to build all radium libs and examples at once
add_custom_target(RadiumLibs DEPENDS Core Engine GuiBase IO PluginBase)

# add a target allowing to force the installation of the library

set(RadiumForceInstallCommutators "")
if (MSVC)
        set(RadiumForceInstallCommutators "/p:Configuration=${CMAKE_BUILD_TYPE}")
endif(MSVC)
add_custom_target(ForceRadiumInstallation
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target install -- ${RadiumForceInstallCommutators}
    COMMENT "Forcing Radium installation")

# Testing
if (RADIUM_ENABLE_TESTING)
    enable_testing()
    include(CTest)
    add_subdirectory(tests)
    add_dependencies(RadiumLibs CoreExampleApp RadiumExampleApps)
endif ()

# Packaging stuff (deb, rpm, windows installer)
#add_subdirectory(packaging)

# https://stackoverflow.com/questions/37434946/how-do-i-iterate-over-all-cmake-targets-programmatically
function(get_all_targets var)
    set(targets)
    get_all_targets_recursive(targets ${CMAKE_CURRENT_SOURCE_DIR})
    set(${var} ${targets} PARENT_SCOPE)
endfunction()

macro(get_all_targets_recursive targets dir)
    get_property(subdirectories DIRECTORY ${dir} PROPERTY SUBDIRECTORIES)
    foreach(subdir ${subdirectories})
        get_all_targets_recursive(${targets} ${subdir})
    endforeach()

    get_property(current_targets DIRECTORY ${dir} PROPERTY BUILDSYSTEM_TARGETS)

    foreach(target ${current_targets})
        get_target_property(type ${target} TYPE)
        if(${type} STREQUAL "EXECUTABLE" OR ${type} STREQUAL "SHARED_LIBRARY")
            list(APPEND ${targets} ${target})
        endif()
    endforeach()
endmacro()


if(ENABLE_COVERAGE)
    set(LCOV_REMOVES "'/usr/*';'*external*';'*install*';'*/Catch2/*';'*_autogen/*';'*build*'")
    add_custom_target(lcov-init
        COMMAND ${LCOV_BIN} --initial --capture --directory ${CMAKE_BINARY_DIR} --output-file ${CMAKE_BINARY_DIR}/init.info
        COMMAND ${LCOV_BIN} --remove ${CMAKE_BINARY_DIR}/init.info ${LCOV_REMOVES} --output-file ${CMAKE_BINARY_DIR}/init.info
        BYPRODUCTS ${CMAKE_BINARY_DIR}/init.info)
    add_custom_target(lcov-zerocounter
        COMMAND ${LCOV_BIN} --zerocounter --directory ${CMAKE_BINARY_DIR})
    add_custom_target(lcov-capture
        COMMAND ${LCOV_BIN} --capture --directory . --output-file  ${CMAKE_BINARY_DIR}/coverage.info
        COMMAND ${LCOV_BIN} --remove coverage.info  ${LCOV_REMOVES} --output-file  ${CMAKE_BINARY_DIR}/coverage.info
        COMMAND ${LCOV_BIN} -a ${CMAKE_BINARY_DIR}/init.info -a ${CMAKE_BINARY_DIR}/coverage.info -o ${CMAKE_BINARY_DIR}/total.info
        BYPRODUCTS ${CMAKE_BINARY_DIR}/total.info)
    add_custom_target(lcov-list
        COMMAND ${LCOV_BIN} --list ${CMAKE_BINARY_DIR}/total.info
        SOURCES  ${CMAKE_BINARY_DIR}/total.info)
    add_custom_target(lcov
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all --parallel
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target lcov-init --parallel
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target lcov-zerocounter --parallel
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target check --parallel
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target lcov-capture --parallel)
endif()

install(FILES LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT meta)
install(FILES README.md DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT meta)

#-------------------------------------------------------------------------------
# Wrap up of settings printed on build

messageTitle(" Final overview for ${PROJECT_NAME} ")
messageInfo("Version:               ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} ${VERSION_TYPE} @ ${VERSION_HOST}")
messageInfo("Install prefix: ")
messageInfo("  ${CMAKE_INSTALL_PREFIX}")
messageInfo("Compiler:              ${CMAKE_CXX_COMPILER}")
messageSetting("CMAKE_BUILD_TYPE")
messageInfo("  possible options: Debug Release RelWithDebInfo MinSizeRel")
messageInfo("  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `")
messageSetting("RADIUM_ENABLE_TESTING")
messageSetting("RADIUM_ENABLE_COVERAGE")
messageSetting("RADIUM_GENERATE_LIB_CORE")
messageSetting("RADIUM_GENERATE_LIB_ENGINE")
messageSetting("RADIUM_GENERATE_LIB_GUIBASE")
messageSetting("RADIUM_GENERATE_LIB_IO")
messageSetting("RADIUM_GENERATE_LIB_PLUGINBASE")
messageSetting("RADIUM_GIT_UPDATE_SUBMODULE")
messageSetting("RADIUM_IO_ASSIMP")
messageSetting("RADIUM_IO_TINYPLY")
messageSetting("RADIUM_IO_DEPRECATED")
messageSetting("RADIUM_SKIP_CORE_EXTERNAL")
messageSetting("RADIUM_SKIP_ENGINE_EXTERNAL")
messageSetting("RADIUM_SKIP_IO_EXTERNAL")
messageSetting("RADIUM_INSTALL_DOC")
messageSetting("RADIUM_UPDATE_VERSION")
messageSetting("RADIUM_QUIET")
messageSetting("USE_GOLD_LINKER")
messageEnd()

set( CACHED_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}" CACHE INTERNAL "Previous value of CMAKE_INSTALL_PREFIX")


