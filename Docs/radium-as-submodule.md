# How to use Radium-Engine as an external dynamic lib for your project

## Fork from AppExample

You can start by forking https://github.com/STORM-IRIT/Radium-AppExample.
It already contains Radium as a submodule and a `build-radium.sh` script
in the external directory.

## Radium as a submodule of your own project

You can clone Radium, or add it as a submodule of your git project.
For instance if you want Radium in `myproject/external/Radium-Engine`

```bash
mkdir external
git submodule add https://github.com/STORM-IRIT/Radium-Engine/ 
external/Radium-Engine --recursive
git submodule update --init --recursive
```

This corresponds to 
```
.gitmodules
[submodule "Radium-Engine"]
        path = external/Radium-Engine
        url = https://github.com/STORM-IRIT/Radium-Engine
        branch = master
```


## Add Radium to your project using CMake `find_package` and out-of-project build
Add the following line to include the Radium-Engine cmake module in your main
`CMakeLists.txt`:
```cmake
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/Radium-Engine/cmake)
```

Add the following line in the `CMakeLists.txt` of your application that need Radium as a library
```cmake
find_package(Radium REQUIRED)
```
Setting the `RADIUM_ROOT` variable before the `find_package`command might be needed to help cmake find the Radium directories:
```cmake
set(RADIUM_ROOT "../external/Radium-Engine")
```

The find package script will will define the following cmake variables:
```cmake
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    ${RADIUM_LIB_DIR}
```
This is how you should add Radium-related includes in your project's `CMakeLists.txt`
```cmake
include_directories(
    ${RADIUM_INCLUDE_DIR} 
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
)
```

And this is how you add the Radium libraries to your linker (with the dependent libraries):
```cmake
# Libs directory include
link_directories(
    ${RADIUM_LIB_DIR}
)
```

```cmake
# Link libraries
target_link_libraries( ${EXEC_FILE}  # target
    ${RADIUM_LIBRARIES}              # Radium libs
    ${GLBINDING_LIBRARIES}           # GLbinding
    ${Qt5_LIBRARIES}                 # QT libraries
)
```

Then build Radium (here, in debug) using the following commands (for Linux):
```bash
cd external
mkdir build-radium-debug
cd build-radium-debug
cmake ../Radium-Engine -DCMAKE_BUILD_TYPE=Debug
make -j8
```

## Add Radium to your project using `ExternaProject_Add` and in-project build.
You can add Radium to your project cmake process with `ExternalProject_Add`, customizing some options.

```cmake
include(ExternalProject)
option(MY_WITH_OMP              "Use OpenMP" ON)
option(MY_WARNINGS_AS_ERRORS    "Treat compiler warning as errors" OFF)
option(MY_ASSIMP_SUPPORT        "Enable assimp loader" ON)
option(MY_TINYPLY_SUPPORT       "Enable TinyPly loader" OFF)
option(MY_BUILD_APPS            "Choose to build or not radium applications" OFF)
option(MY_FAST_MATH "Enable Fast Math optimizations in Release Mode (ignored with MVSC)" OFF)

# add Radium with the options you want to override
ExternalProject_Add(
        Radium
        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Radium-Engine"
        # set the compilation options
        CMAKE_ARGS
            -DRADIUM_WARNINGS_AS_ERRORS=${MY_WARNINGS_AS_ERRORS}
            -DRADIUM_ASSIMP_SUPPORT=${MY_ASSIMP_SUPPORT}
            -DRADIUM_TINYPLY_SUPPORT=${MY_TINYPLY_SUPPORT}
            -DRADIUM_BUILD_APPS=${MY_BUILD_APPS}
            -DRADIUM_FAST_MATH=${MY_FAST_MATH}
            -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
            -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
	# skip the install step	
	INSTALL_COMMAND cmake -E echo "Skipping install step."
)
```
Then you have to setup manually all the variables that would have been set up by `find_package`:
```cmake
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    ${RADIUM_LIB_DIR}
    ${RADIUM_LIBRARIES}
    ${GLBINDING_LIBRARIES}
```
For instance :
```cmake
set( CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/external/Radium-Engine/cmake )
set( RADIUM_ROOT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Radium-Engine" )

set( EIGEN3_INCLUDE_DIR  "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/3rdPartyLibraries/include")
set( GLM_INCLUDE_DIR     "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/3rdPartyLibraries/include")
set( ASSIMP_LIBRARIES    "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/3rdPartyLibraries/lib/libassimp.so")
set( GLBINDING_LIBRARIES "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/3rdPartyLibraries/lib/libglbinding.so")
set( GLOBJECTS_LIBRARIES "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/3rdPartyLibraries/lib/libglobjects.so")
set( OPENMESH_LIBRARIES  "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/3rdPartyLibraries/lib/libOpenMeshCore.so")
set( RADIUM_PLUGIN_OUTPUT_PATH "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}//${CMAKE_BUILD_TYPE}/bin/Plugins" )
set( RADIUM_INCLUDE_DIRS )
list(APPEND RADIUM_INCLUDE_DIRS "${RADIUM_ROOT_DIR}/src" "${EIGEN3_INCLUDE_DIR}" "${ASSIMP_INCLUDE_DIR}" "${GLBINDING_INCLUDE_DIR}" "${GLOBJECTS_INCLUDE_DIR}")

set( RA_CORE_LIB    "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE}/lib/libradiumCore.so" )
set( RA_ENGINE_LIB  "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE}/lib/libradiumEngine.so" )
set( RA_IO_LIB      "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE}/lib/libradiumIO.so" )
set( RA_GUIBASE_LIB "${RADIUM_ROOT_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}/${CMAKE_BUILD_TYPE}/lib/libradiumGuiBase.so" )
set( RADIUM_LIBRARIES  )
list(APPEND RADIUM_LIBRARIES "${RA_CORE_LIB}" "${RA_ENGINE_LIB}" "${RA_IO_LIB}" "${RA_GUIBASE_LIB}")
```
