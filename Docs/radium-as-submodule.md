# How to use Radium-Engine as an external dynamic lib for your project

## Fork pre made AppExample

You can also fork https://github.com/STORM-IRIT/Radium-AppExample 
It already contains Radium as submodule and a build-radium.sh script
in the external directory.

## git submodule, or just clone

You can clone Radium, or add it as a submodule of your git project
git submodule radium in my own project
For instance Radium in ./myproject/external/Radium-Engine

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


## Add Radium to the lib of your project
Add a line to include the Radium-Engine cmake module in your main
CMakeLists.txt
```
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/external/Radium-Engine/CMakeModules)
```


add a ``find_package(Radium REQUIRED)`` in CMakeLists.txt of your
applications that need. If needed add ``set(RADIUM_ROOT "../external/Radium-Engine")``

This will def the following cmake vars 
```
    ${RADIUM_INCLUDE_DIR}
    ${EIGEN_INCLUDE_DIR}
    ${ASSIMP_INCLUDE_DIR}
    ${RADIUM_LIB_DIR}
```


access to Radium headers and declarations/defintions
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
    ${RADIUM_LIB_CORE}              # core
    ${RADIUM_LIB_ENGINE}            # engine
    ${RADIUM_LIB_GUIBASE}           # gui
    ${Qt5_LIBRARIES}                # the Qt beast
)
```

Then build radium (as debug and release)
For linux something like
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
