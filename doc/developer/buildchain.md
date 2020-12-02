\page develbuildchain Radium Build Chain
[TOC]

# Installation directory
By default, `${CMAKE_INSTALL_PREFIX}` is set as follow:

- For release build :

~~~{.cmake}
    set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID})
~~~

- For Debug or RelWithDebInfo build

~~~{.cmake}
    set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE})
~~~

# Radium Libraries
## Overview
Radium is split in 5 libraries: Core, Engine, GuiBase, UI and PluginBase.
The compilation of each library is controlled by the following cmake options

~~~{.cmake}
option(RADIUM_GENERATE_LIB_CORE       "Include Radium::Core in CMake project" ON)
option(RADIUM_GENERATE_LIB_IO         "Include Radium::IO in CMake project" ON)
option(RADIUM_GENERATE_LIB_ENGINE     "Include Radium::Engine in CMake project" ON)
option(RADIUM_GENERATE_LIB_GUIBASE    "Include Radium::GuiBase in CMake project" ON)
option(RADIUM_GENERATE_LIB_PLUGINBASE "Include Radium::PluginBase in CMake project" ON)
~~~

## Dependencies between libraries

~~~{.cmake}
add_dependencies (${ra_engine_target} PUBLIC Radium::Core)
add_dependencies (${ra_io_target} PUBLIC Radium::Core)
add_dependencies (${ra_pluginbase_target} Radium::Core Radium::Engine)
add_dependencies (${ra_guibase_target} PUBLIC Radium::Core Radium::Engine Radium::PluginBase Radium::IO)
~~~

\warning Consistency of `RADIUM_GENERATE_LIB_***` options is not checked wrt. the dependencies.

- When enabled using `RADIUM_GENERATE_LIB_***`, each library has a compilation target: `Core`, `Engine`, ...

# External dependencies

External dependencies can be fetched and built at configure time or pre build once for allowing

## Fetch and build at configure time

Each library comes with its own dependencies, which are fetched and built at **configure** time:

~~~{.sh}
$ cmake .. -DRADIUM_GENERATE_LIB_IO=OFF -DRADIUM_GENERATE_LIB_ENGINE=OFF -DRADIUM_GENERATE_LIB_GUIBASE=OFF -DRADIUM_GENERATE_LIB_PLUGINBASE=OFF -DRADIUM_ENABLE_TESTING=OFF
Starting to parse CMake project.
Externals will be built with 8 core(s)
    == radiumproject Project configuration ==
USE_GOLD_LINKER                           OFF
Git Changeset: a65ec5d06d8aa207080a42935efd10435f230c38
Can build doc?                            YES
  + Graphviz/Dot - for generated graphs   YES
Sanitizers:
  + ADDRESS_SANITIZER                     OFF
  + UB_SANITIZER                          OFF
  + THREAD_SANITIZER                      OFF
[addExternalFolder] process Core /*****/Radium-Engine/src/Core/external
[addExternalFolder] Create temporary directory
[addExternalFolder] Configure cmake project
[addExternalFolder] Start build
[addExternalFolder] Build ended
[addExternalFolder] Configure package file /****/build-Radium-Engine-Desktop-Release/src/Core/external/cmake/package-Core.cmake
Configure library Core with default settings
clang-tidy - static analysis              NO
cppcheck - static analysis                NO
clang-format - code formating             YES
Sanitizers:
  + ADDRESS_SANITIZER                     OFF
  + UB_SANITIZER                          OFF
  + THREAD_SANITIZER                      OFF
    == Final overview for radiumproject ==
Version:               1.0.0 beta @ cezanne
Install prefix:        ****/Bundle-GNU
Compiler:              /usr/bin/g++
CMAKE_BUILD_TYPE:      Release
CMAKE_INSTALL_PREFIX:  ****/Bundle-GNU
RADIUM_ENABLE_TESTING: OFF
  possible options: Debug Release RelWithDebInfo MinSizeRel
  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `
Configuring done
Generating done
CMake Project was parsed successfully.
~~~

What is happening here, is that the dependencies of Core are fetched using git, then
configured using cmake, built, and installed to ${CMAKE_INSTALL_PREFIX} of Radium.
\see File `cmake/externalFunc.cmake` for technical details.

In order to save compilation time, dependencies are processed at the first run, and then skipped.
To force processing again the dependencies of a given library, turn on the associated option

~~~
OPTION( RADIUM_SKIP_${NAME_UPPER}_EXTERNAL "[addExternalFolder] Skip updating ${NAME}::external (disable for rebuild)" ON)
~~~

where `$NAME_UPPER` is `CORE`, `ENGINE`, `GUIBASE`, `PLUGINBASE` or `IO`.
\note The option `RADIUM_SKIP_${NAME_UPPER}_EXTERNAL` is generated only if `RADIUM_GENERATE_LIB_${NAME_UPPER}` is `ON`.


## Building and installing Radium dependencies once for all
Radium dependencies can be built and installed alone, without building the whole Radium project.

The file `external/CMakeLists.txt` could be used as a standalone project to install all the Radium dependencies in any location _outside Radium-Engine source dir_ (if you try to build and install external as a subdir of Radium-Engine source, cmake configuration will ).

Configure dependencies build with :

~~~{.bash}
mkdir BuildRadiumDependencies && cd BuildRadiumDependencies
cmake -DCMAKE_INSTALL_PREFIX=/path/to/external/install /path/to/Radium/external
~~~

If not given on the command line, the installation directory is set by default to, `{CMAKE_CURRENT_BINARY_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE}`.

Then to compile Radium-Engine, you have to give each dependency to the cmake command at configuration step.
One easy way is to store your configuration in a file and use the `-C` option of cmake.

cmake config file `/path/to/myconfig.cmake`:

~~~{.cmake}
#replace this one with you external install path
set(RADIUM_DEP_PREFIX /path/to/external/install)

#set each external dirs
set(Eigen3_DIR      ${RADIUM_DEP_PREFIX}/share/eigen3/cmake/ CACHE PATH "My Eigen")
set(OpenMesh_DIR    ${RADIUM_DEP_PREFIX}/share/OpenMesh/cmake/ CACHE PATH "My OpenMesh")
set(cpplocate_DIR   ${RADIUM_DEP_PREFIX}/share/cpplocate/ CACHE PATH "My cpplocate")
set(glm_DIR         ${RADIUM_DEP_PREFIX}/lib/cmake/glm/ CACHE PATH "My glm")
set(glbinding_DIR   ${RADIUM_DEP_PREFIX}/share/glbinding/ CACHE PATH "My glbinding")
set(globjects_DIR   ${RADIUM_DEP_PREFIX}/share/globjects/ CACHE PATH "My globjects")
set(stb_DIR         ${RADIUM_DEP_PREFIX}/include/stb/ CACHE PATH "My stb")
set(assimp_DIR      ${RADIUM_DEP_PREFIX}/lib/cmake/assimp-5.0/ CACHE PATH "My assimp")
set(tinyply_DIR     ${RADIUM_DEP_PREFIX}/lib/cmake/tinyply/  CACHE PATH "My tinyply")
~~~

and call `cmake -C /path/to/myconfig.cmake  ......`

Another option is to provide each value direclty to the cmake command line with `-D` with:

~~~{.bash}
cmake \
-DEigen3_DIR      /path/to/external/install/share/eigen3/cmake/ \
-DOpenMesh_DIR    /path/to/external/install/share/OpenMesh/cmake/ \
-Dcpplocate_DIR   /path/to/external/install/share/cpplocate/ \
-Dglm_DIR         /path/to/external/install/lib/cmake/glm/ \
-Dglbinding_DIR   /path/to/external/install/share/glbinding/ \
-Dglobjects_DIR   /path/to/external/install/share/globjects/ \
-Dstb_DIR         /path/to/external/install/include/stb/ \
-Dassimp_DIR      /path/to/external/install/lib/cmake/assimp-5.0/ \
-Dtinyply_DIR     /path/to/external/install/lib/cmake/tinyply/ \
.......
~~~

## User provided external dependencies

You can  use your own installation of a local dependency instead of letting cmake fetch and compile it.
To this end, just provide the corresponding '*_DIR' to cmake at configuration time (with '-D' option, configuration file or toolchain file, please refer to cmake documentation).

Currently supported (note that these paths must refer to the installation directory of the corresponding library):
[//]: # (generated running ../script/list_dep.py from Radium-Engine/external directory)

 *  `assimp_DIR`
 *  `tinyply_DIR`
 *  `glm_DIR`
 *  `glbinding_DIR`
 *  `globjects_DIR`
 *  `stb_DIR`
 *  `Eigen3_DIR`
 *  `OpenMesh_DIR`
 *  `cpplocate_DIR`


Radium is compiled and tested with specific version of dependencies, as given in the external's folder CMakeLists.txt and state here for the record


 *  assimp: https://github.com/assimp/assimp.git, [tags/v5.0.1],
    *  with options `-DASSIMP_BUILD_ASSIMP_TOOLS=False -DASSIMP_BUILD_SAMPLES=False -DASSIMP_BUILD_TESTS=False -DIGNORE_GIT_HASH=True -DASSIMP_NO_EXPORT=True`
 *  tinyply: https://github.com/ddiakopoulos/tinyply.git, [tags/2.3.2],
    *  with options `-DSHARED_LIB=TRUE`
 *  glm: https://github.com/g-truc/glm.git, [0.9.9.5],
    *  with options `-DGLM_TEST_ENABLE=OFF -DBUILD_STATIC_LIBS=OFF -DCMAKE_INSTALL_LIBDIR=lib`
 *  glbinding: https://github.com/cginternals/glbinding.git, [663e19cf1ae6a5fa1acfb1bd952fc43f647ca79c],
    *  with options `-DOPTION_BUILD_TESTS=OFF -DOPTION_BUILD_DOCS=OFF -DOPTION_BUILD_TOOLS=OFF -DOPTION_BUILD_EXAMPLES=OFF`
 *  globjects: https://github.com/dlyr/globjects.git, [11c559a07d9e310abb2f53725fd47cfaf538f8b1],
    *  with options `-DOPTION_BUILD_DOCS=OFF -DOPTION_BUILD_EXAMPLES=OFF -DOPTION_USE_EIGEN=ON -Dglbinding_DIR=${glbinding_DIR} -Dglm_DIR=${glm_DIR} -DEigen3_DIR=${Eigen3_DIR}`
 *  stb: https://github.com/nothings/stb.git, [1034f5e5c4809ea0a7f4387e0cd37c5184de3cdd],
    *  with options `None`
 *  Eigen3: https://gitlab.com/libeigen/eigen.git, [e80ec243],
    *  with options `-DEIGEN_TEST_CXX11=OFF -DBUILD_TESTING=OFF`
 *  OpenMesh: https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git, [tags/OpenMesh-8.1],
    *  with options `-DBUILD_APPS=OFF`
 *  cpplocate: https://github.com/cginternals/cpplocate.git, [tags/v2.2.0],
    *  with options `-DOPTION_BUILD_TESTS=OFF -DOPTION_BUILD_DOCS=OFF`

[//]: # (end script copy)

\warning You have to take care of the consistency of the external dependencies, e.g. it's not possible to use your version of globjects without providing your version of eigen, otherwise you will have mixed version in Radium.

