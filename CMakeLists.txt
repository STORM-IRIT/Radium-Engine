cmake_minimum_required (VERSION 3.6)

#------------------------------------------------------------------------------
# Policies
if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
endif()

# Project setup, versioning stuff here, change when changing the version
# Note: keep the project name lower case only for easy linux packaging support
project (radiumproject VERSION 1.0.0)
set(VERSION_TYPE "beta" CACHE STRING "version type" FORCE)
site_name(VERSION_HOST) # read hostname to VERSION_HOST
set(VERSION_HOST "${VERSION_HOST}" CACHE STRING "host of build" FORCE)

message(STATUS "")
message(STATUS "    == ${PROJECT_NAME} Project configuration ==")
message(STATUS "")

#------------------------------------------------------------------------------
# General settings

OPTION( CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT "Install by default to Bundle" ON)

# Be nice to visual studio
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Be nice and export compile commands by default, this is handy for clang-tidy
# and for other tools.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# We can use include() and find_package() for our scripts in there
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# Use gold linker to speed up linking time, see cmake/useGoldLinker.cmake
include(useGoldLinker)

# Helpful option enable build profiling to identify slowly compiling files
option(MEASURE_ALL "When enabled all commands will be passed through time command" OFF)
if(MEASURE_ALL)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "time")
endif()

#-------------------------------------------------------------------------------
# Set default paths for Radium
set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID})
set(RADIUM_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

#-------------------------------------------------------------------------------
# Set default install location to dist folder in build dir
# we do not want to install to /usr by default
if (NOT CMAKE_INSTALL_PREFIX AND CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set (CMAKE_INSTALL_PREFIX "${RADIUM_BUNDLE_DIRECTORY}" CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE )
    message( "Set install prefix to ${CMAKE_INSTALL_PREFIX}" )
endif()


#-------------------------------------------------------------------------------
# Set default install location to dist folder in build dir
# we do not want to install to /usr by default
if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE Release )
    message( "Set default build type to ${CMAKE_INSTALL_PREFIX}" )
endif()

#------------------------------------------------------------------------------
# Custom Install target, used in run target in source/CMakeLists.txt
if (CMAKE_GENERATOR MATCHES "Makefiles")
    # Make it multithreaded
    add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target
        install -- -j WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
else()
    add_custom_target( Install_ COMMAND "${CMAKE_COMMAND}" --build . --target
        install WORKING_DIRECTORY "${CMAKE_BINARY_DIR}")
endif()


#------------------------------------------------------------------------------
# Included CMakeLists.txt

# External resources/repositories are downloaded and built at configuration stage
message(STATUS "[External] Create compilation directory")
execute_process(
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/external
)
message(STATUS "[External] Run cmake")
execute_process(
    COMMAND ${CMAKE_COMMAND} ${CMAKE_CURRENT_SOURCE_DIR}/external -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external
)
message(STATUS "[External] Build started")
execute_process(
    COMMAND ${CMAKE_COMMAND} --build . --target all
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/external
)
message(STATUS "[External] Build ended")

#This dummy target add external/CMakeLists.txt to most IDEs
add_custom_target(external ALL
    SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/external/CMakeLists.txt)

# TODO: temporary patch: these paths need to be exported at install time by external package
set(Eigen3_DIR ${CMAKE_INSTALL_PREFIX}/share/eigen3/cmake/)
set(OpenMesh_DIR ${CMAKE_INSTALL_PREFIX}/share/OpenMesh/cmake/)

# Documentation build
#add_subdirectory(doc)

# Images, databases and other data which needs to be installed for project
#add_subdirectory(data)

# Testing
#enable_testing()
#add_subdirectory(test)

# Source code
add_subdirectory(src)

# Packaging stuff (deb, rpm, windows installer)
#add_subdirectory(packaging)



#-------------------------------------------------------------------------------
# Wrap up of settings printed on build
message(STATUS "")
message(STATUS "    == Final overview for ${PROJECT_NAME} ==")
message(STATUS "Version:              ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH} ${VERSION_TYPE} @ ${VERSION_HOST}")
message(STATUS "Install prefix:       ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Compiler:             ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_BUILD_TYPE:     ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_INSTALL_PREFIX: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "  possible options: Debug Release RelWithDebInfo MinSizeRel")
message(STATUS "  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `")
message(STATUS "")


