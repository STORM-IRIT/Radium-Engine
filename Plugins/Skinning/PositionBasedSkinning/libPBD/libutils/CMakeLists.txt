cmake_minimum_required(VERSION 2.8)
project(utils)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/../../CMakeModules)
find_package( Radium REQUIRED )
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${RADIUM_ROOT}/cmake)
include (CompileFlags)

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

add_library(utils STATIC
        *.h
        *cpp
        CMakeLists.txt
)

include_directories(
   "${CMAKE_CURRENT_SOURCE_DIR}"
   "${CMAKE_CURRENT_SOURCE_DIR}/.."
   "${RADIUM_ROOT}/3rdPartyLibraries/Eigen"
)
