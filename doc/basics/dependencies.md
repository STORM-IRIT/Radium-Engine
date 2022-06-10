\page dependenciesmanagement Radium Dependencies Management
[TOC]

Radium relies on several external libraries to load files or to represent some data:

* [Core] Eigen, OpenMesh, nlohmann_json
* [Engine] glm, globjects, glbindings, tinyEXR
* [IO] Assimp
* [Gui] Qt Core, Qt Widgets and Qt OpenGL v5.5+ (5.14 at least, Qt6 support is experimental), PowerSlider
* [doc] Doxygen-awesome-css
* stb_image

We developed a series of tools to fetch and compile these dependencies easily, except for
Qt, which needs to be installed and passed to cmake through the variables `CMAKE_PREFIX_PATH` or `Qt5_DIR` OR `Qt6_DIR`
(see documentation at <https://doc.qt.io/qt-5.15/cmake-manual.html#getting-started>).

# Dependencies management systems

We offer two different systems to handle external dependencies (see details and how-to in the following sections):

 1. **[recommended]** external build: the user compiles and installs once for all the dependencies using a dedicated
    cmake project. Then, Radium cmake project is configured to link with installed dependencies.
 2. manual management: users can also provide their own version of the dependencies through cmake packages.

\warning Dependencies need to be built with the same build type (ie. Release, Debug) than Radium.

# Building and installing Radium dependencies once for all {#builddep}

We provide a standalone cmake project (`external/CMakeLists.txt`) to compile and install the Radium dependencies at any location.

## Configuration and compilation of the dependencies

External dependencies have to be installed outside Radium-Engine source tree.

~~~{.bash}
# from wherever you want outside radium source tree
cmake Radium-Source-Tree/external -DCMAKE_BUILD_TYPE=Release -B build-r/ -DCMAKE_INSTALL_PREFIX=`pwd`/install-r
cmake --build build-r --parallel

cmake cmake Radium-Source-Tree/external -DCMAKE_BUILD_TYPE=Debug -B build-d/ -DCMAKE_INSTALL_PREFIX=`pwd`/install-d
cmake --build build-d --parallel
~~~

If not given on the command line, the installation directory is set by default to `{CMAKE_CURRENT_BINARY_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}` for `CMAKE_BUILD_TYPE=Release`, and  `{CMAKE_CURRENT_BINARY_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE}` for any other `CMAKE_BUILD_TYPE`.

### Getting started with Visual Studio

Open `external/CMakeLists.txt` and edit cmake settings or `CMakeSettings.json`. External build and install have to be outside Radium-Engine source directory. For instance

~~~{.json}
{
  "configurations": [
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "inheritEnvironments": [ "msvc_x64_x64" ],
      "buildRoot": "${projectDir}/../../radium-externals/build/${name}",
      "installRoot": "${projectDir}/../../radium-externals/install/${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": ""
    },
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "RelWithDebInfo",
      "buildRoot": "${projectDir}/../../radium-externals/build/${name}",
      "installRoot": "${projectDir}/../../radium-externals/install/${name}",
      "cmakeCommandArgs": "",
      "buildCommandArgs": "",
      "ctestCommandArgs": "",
      "inheritEnvironments": [ "msvc_x64_x64" ],
      "variables": []
    }
  ]
}
~~~

## Configuration of Radium

To compile Radium-Engine, you have to indicate where cmake can find each dependency.

For convenience, dependencies install procedure create a configuration file, you can use this configuration with the `-C` option of cmake.
The cmake config file `/path/to/external/install/radium-options.cmake` contains, depending on your dependency configuration:

~~~{.cmake}
set(RADIUM_DEP_PREFIX "/path/to/external/install/")
set(Eigen3_DIR        "${RADIUM_DEP_PREFIX}/share/eigen3/cmake/" CACHE PATH "My Eigen")
set(OpenMesh_DIR      "${RADIUM_DEP_PREFIX}/share/OpenMesh/cmake/" CACHE PATH "My OpenMesh")
set(cpplocate_DIR     "${RADIUM_DEP_PREFIX}/share/cpplocate/" CACHE PATH "My cpplocate")
set(glm_DIR           "${RADIUM_DEP_PREFIX}/lib/cmake/glm/" CACHE PATH "My glm")
set(glbinding_DIR     "${RADIUM_DEP_PREFIX}/share/glbinding/" CACHE PATH "My glbinding")
set(globjects_DIR     "${RADIUM_DEP_PREFIX}/share/globjects/" CACHE PATH "My globjects")
set(stb_DIR           "${RADIUM_DEP_PREFIX}/include/stb/" CACHE PATH "My stb")
set(assimp_DIR        "${RADIUM_DEP_PREFIX}/lib/cmake/assimp-5.0/" CACHE PATH "My assimp")
set(tinyply_DIR       "${RADIUM_DEP_PREFIX}/lib/cmake/tinyply/" CACHE PATH "My tinyply")
set(tinyEXR_DIR       "${RADIUM_DEP_PREFIX}/share/tinyEXR/cmake/"  CACHE PATH "My tinyEXR")
set(nlohmann_json_DIR "${RADIUM_DEP_PREFIX}/lib/cmake/nlohmann_json/" CACHE PATH "My json")
set(PowerSlider_DIR   "${RADIUM_DEP_PREFIX}/lib/cmake/PowerSlider/" CACHE PATH "My PowerSlider")
set(RADIUM_IO_ASSIMP   ON CACHE BOOL "Radium uses assimp io")
set(RADIUM_IO_TINYPLY  ON CACHE BOOL "Radium uses tinyply io")
~~~

When configuring Radium cmake project, don't forget to add this file by calling `cmake -C /path/to/external/install/radium-options.cmake  ......`

You can also provide these variables as cmake command line argument:

~~~{.bash}
cmake \
-DEigen3_DIR        /path/to/external/install/share/eigen3/cmake/ \
-DOpenMesh_DIR      /path/to/external/install/share/OpenMesh/cmake/ \
-Dcpplocate_DIR     /path/to/external/install/share/cpplocate/ \
-Dnlohmann_json_DIR /path/to/external/install/lib/cmake/nlohmann_json/ \
-Dglm_DIR           /path/to/external/install/lib/cmake/glm/ \
-Dglbinding_DIR     /path/to/external/install/share/glbinding/ \
-Dglobjects_DIR     /path/to/external/install/share/globjects/ \
-DPowerSlider_DIR   /path/to/external/install/lib/cmake/PowerSlider \
-Dstb_DIR           /path/to/external/install/include/stb/ \
-Dassimp_DIR        /path/to/external/install/lib/cmake/assimp-5.0/ \
-Dtinyply_DIR       /path/to/external/install/lib/cmake/tinyply/ \
-DtinyEXR_DIR       /path/to/external/install/share/tinyEXR/cmake/ \
.......
~~~

# User provided external dependencies

You can  use your own installation of a local dependency instead of letting cmake fetch and compile it.
To this end, just provide the corresponding '*_DIR' to cmake at configuration time as show before (with '-D' option, configuration file or toolchain file, please refer to cmake documentation).

Currently supported (note that these paths must refer to the installation directory of the corresponding library):
<!--  (generated running ../script/list_dep.py from Radium-Engine/external directory) -->

* `Eigen3_DIR`
* `OpenMesh_DIR`
* `cpplocate_DIR`
* `nlohmann_json_DIR`
* `assimp_DIR`
* `tinyply_DIR`
* `PowerSlider_DIR`
* `glm_DIR`
* `glbinding_DIR`
* `globjects_DIR`
* `stb_DIR`
* `tinyEXR_DIR`

Radium is compiled and tested with specific version of dependencies, as given in the external's folder CMakeLists.txt and state here for the record

* Eigen3: https://gitlab.com/libeigen/eigen.git, [e80ec243],
  * with options `-DEIGEN_TEST_CXX11=OFF -DBUILD_TESTING=OFF -DEIGEN_BUILD_DOC=OFF`
* OpenMesh: https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git, [tags/OpenMesh-8.1],
  * with options `-DBUILD_APPS=OFF`
* cpplocate: https://github.com/cginternals/cpplocate.git, [tags/v2.2.0],
  * with options `-DOPTION_BUILD_TESTS=OFF -DOPTION_BUILD_DOCS=OFF`
* nlohmann_json: https://github.com/nlohmann/json.git, [release/3.10.5],
  * with options `-DJSON_Install=ON -DJSON_BuildTests=OFF`
* assimp: https://github.com/assimp/assimp.git, [tags/v5.0.1],
  * with options `-DASSIMP_BUILD_ASSIMP_TOOLS=False -DASSIMP_BUILD_SAMPLES=False -DASSIMP_BUILD_TESTS=False -DIGNORE_GIT_HASH=True -DASSIMP_NO_EXPORT=True`
* tinyply: https://github.com/ddiakopoulos/tinyply.git, [tags/2.3.2],
  * with options `-DSHARED_LIB=TRUE`
* PowerSlider: https://github.com/dlyr/PowerSlider.git, [origin/master],
  * with options `-DBUILD_DESIGNER_PLUGIN=OFF -DBUILD_EXAMPLE_APP=OFF`
* glm: https://github.com/g-truc/glm.git, [0.9.9.5],
  * with options `-DGLM_TEST_ENABLE=OFF -DBUILD_STATIC_LIBS=OFF -DCMAKE_INSTALL_LIBDIR=lib`
* glbinding: https://github.com/cginternals/glbinding.git, [663e19cf1ae6a5fa1acfb1bd952fc43f647ca79c],
  * with options `-DOPTION_BUILD_TESTS=OFF -DOPTION_BUILD_DOCS=OFF -DOPTION_BUILD_TOOLS=OFF -DOPTION_BUILD_EXAMPLES=OFF`
* globjects: https://github.com/dlyr/globjects.git, [11c559a07d9e310abb2f53725fd47cfaf538f8b1],
  * with options `-DOPTION_BUILD_DOCS=OFF -DOPTION_BUILD_EXAMPLES=OFF -DOPTION_USE_EIGEN=ON -Dglbinding_DIR=${glbinding_DIR} -Dglm_DIR=${glm_DIR} -DEigen3_DIR=${Eigen3_DIR}`
* stb: https://github.com/nothings/stb.git, [1034f5e5c4809ea0a7f4387e0cd37c5184de3cdd],
  * with options `None`
* tinyEXR: https://github.com/MathiasPaulin/tinyexr.git, [origin/radium],
  * with options `-DBUILD_INSTALLABLE_LIB=ON`

<!--  (end script copy) -->

\warning You have to take care of the consistency of the external dependencies, e.g. it's not possible to use your version of globjects without providing your version of eigen, otherwise you will have mixed version in Radium.
