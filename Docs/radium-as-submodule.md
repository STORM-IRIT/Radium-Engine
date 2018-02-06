# How to use Radium-Engine as an external dynamic lib for your project

## Fork pre made AppExample

You can also fork https://github.com/STORM-IRIT/Radium-AppExample 
It already contains Radium as submodule and a build-radium.sh script
in the external directory.

## git submodule, or just clone

You can clone Radium, or add it as a submodule of your git project for instance Radium in ./myproject/external/Radium-Engine

```
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


## Add Radium to your project using `find_package` and out of project build
Add the following line to include the Radium-Engine cmake module in your main
CMakeLists.txt
```
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/Radium-Engine/cmake)
```


Add the following line in the `CMakeLists.txt` of your application that need Radium as a library
``find_package(Radium REQUIRED)``.
Setting the `RADIUM_ROOT` variable before the `find_package`command migh be needed to help cmake find the Radium library.
``set(RADIUM_ROOT "../external/Radium-Engine")``.

This will define the following cmake variables:
```
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    ${RADIUM_LIB_DIR}
```


and give access to Radium headers and declarations/definitions through the following commands:
```
include_directories(
    .
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
)
```

```
# Libs include
link_directories(
    ${RADIUM_LIB_DIR}
)
```

```
# Link good libraries
target_link_libraries( ${EXEC_FILE} # target
    ${RADIUM_LIBRARIES}              # Radium libs
    ${GLBINDING_LIBRARIES}           # Radium dep
    ${Qt5_LIBRARIES}                 # the Qt beast
)
```

Then build Radium (in debug and release) using the following commands (for Linux):
```
cd external
mkdir build-radium-debug
cd build-radium-debug
cmake ../Radium-Engine -DCMAKE_BUILD_TYPE=Debug
make -j8
cd ..
mkdir build-radium-release
cmake ../Radium-Engine -DCMAKE_BUILD_TYPE=Release
make -j8
```

## Add Radium to your project using `ExternaProject_Add` and in project build.
You can add Radium to your project cmake process with `ExternalProject_Add`, customizing the some options.

```
include(ExternalProject)
option(MY_WITH_OMP              "Use OpenMP" ON)
option(MY_WARNINGS_AS_ERRORS    "Treat compiler warning as errors" ON)
option(MY_ASSIMP_SUPPORT        "Enable assimp loader" ON)
option(MY_TINYPLY_SUPPORT       "Enable TinyPly loader" OFF)
option(MY_BUILD_APPS            "Choose to build or not radium applications" ON)
option(MY_FAST_MATH "Enable Fast Math optimizations in Release Mode (ignored with MVSC)" OFF)

# add Radium with the options you want to override
ExternalProject_Add(
        Radium
        # where the source will live
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/Radium-Engine"
        # set the compilation options
        CMAKE_ARGS
            -DRADIUM_WITH_OMP=${MY_WITH_OMP}
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
Then you have to setup manually all the variables that would have been setted up by `find_package`:
```
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    ${RADIUM_LIB_DIR}
    ${RADIUM_LIBRARIES}
    ${GLBINDING_LIBRARIES}
```
