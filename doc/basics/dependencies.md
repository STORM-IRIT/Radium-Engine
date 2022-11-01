\page dependenciesmanagement Radium Dependencies Management
[TOC]

Radium relies on several external libraries to load files or to represent some data:

* [Core] Eigen, OpenMesh, nlohmann_json
* [Engine] glm, globjects, glbindings, tinyEXR
* [IO] Assimp
* [Gui] Qt Core, Qt Widgets and Qt OpenGL v5.5+ (5.14 at least, Qt6 support is experimental), PowerSlider
* [Dataflow] stduuid, RadiumNodeEditor
* [doc] Doxygen-awesome-css
* stb_image

We developed a series of tools to fetch and compile these dependencies easily, except for
Qt, which needs to be installed and passed to cmake through the variables `CMAKE_PREFIX_PATH` or `Qt5_DIR` `Qt6_DIR`
(see documentation at <https://doc.qt.io/qt-5.15/cmake-manual.html#getting-started>).

# Dependencies management systems

We offer two different systems to handle external dependencies (see details and how-to in the following sections):

 1. **[recommended]** external build: the user compiles and installs once for all the dependencies using a dedicated
    cmake project. Then, Radium cmake project is configured to link with installed dependencies.
 2. manual management: users can also provide their own version of the dependencies through cmake packages.

\warning Dependencies need to be built with the same build type (ie. Release, Debug) than Radium.

# Building and installing Radium dependencies once for all {#builddep}

We provide a standalone cmake project (`Radium-Engine/external/CMakeLists.txt`) to compile and install the Radium dependencies at any location.

## Configuration and compilation of the dependencies

External dependencies have to be installed outside Radium-Engine source tree.

~~~{.bash}
# from wherever you want outside radium source tree
# in release
cmake -S Radium-Engine/external -B builds/radium-external-build-r -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=installs/radium-external-r
cmake --build builds/radium-external-build-r --config Release --parallel

# in debug
cmake -S Radium-Engine/external -B builds/radium-external-build-d -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=installs/radium-external-d
cmake --build builds/radium-external-build-d --config Debug --parallel
~~~

If not given on the command line, the installation directory is set by default to `Radium-Engine/{CMAKE_CURRENT_BINARY_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}` for `CMAKE_BUILD_TYPE=Release`, and  `Radium-Engine/{CMAKE_CURRENT_BINARY_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE}` for any other `CMAKE_BUILD_TYPE`.

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
      "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=C:/Qt/6.3.0/msvc2019_64",
      "buildCommandArgs": "",
      "ctestCommandArgs": ""
    },
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "Release",
      "buildRoot": "${projectDir}/../../radium-externals/build/${name}",
      "installRoot": "${projectDir}/../../radium-externals/install/${name}",
      "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=C:/Qt/6.3.0/msvc2019_64",
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
The cmake config file `/path/to/installs/radium-external-r/radium-options.cmake` contains, depending on your dependency configuration:

~~~{.cmake}
set(Eigen3_DIR "/path/to/external/install/share/eigen3/cmake/" CACHE PATH "My Eigen3 location")
set(OpenMesh_DIR "/path/to/external/install/share/OpenMesh/cmake/" CACHE PATH "My OpenMesh location")
set(cpplocate_DIR "/path/to/external/install/share/cpplocate/" CACHE PATH "My cpplocate location")
set(nlohmann_json_DIR "/path/to/external/install/lib/cmake/nlohmann_json/" CACHE PATH "My nlohmann_json location")
set(glm_DIR "/path/to/external/install/glm/lib/cmake/glm/" CACHE PATH "My glm location")
set(glbinding_DIR "/path/to/external/install/glbinding/" CACHE PATH "My glbinding location")
set(globjects_DIR "/path/to/external/install/globjects/" CACHE PATH "My globjects location")
set(stb_DIR "/path/to/external/install/include/stb/" CACHE PATH "My stb location")
set(stb_INCLUDE_DIR "/path/to/external/install/include/" CACHE PATH "My stb_INCLUDE location")
set(tinyEXR_DIR "/path/to/external/install/share/tinyEXR/cmake/" CACHE PATH "My tinyEXR location")
set(assimp_DIR "/path/to/external/install/lib/cmake/assimp-5.0/" CACHE PATH "My assimp location")
set(tinyply_DIR "/path/to/external/install/lib/cmake/tinyply/" CACHE PATH "My tinyply location")
set(PowerSlider_DIR "/path/to/external/install/lib/cmake/PowerSlider/" CACHE PATH "My PowerSlider location")
set(stduuid_DIR "/path/to/external/install/lib/cmake/stduuid/" CACHE PATH "My stduuid")
set(RadiumNodeEditor_DIR "/path/to/external/install/lib/cmake/RadiumNodeEditor/" CACHE PATH "My NodeEditor")
set(RADIUM_IO_ASSIMP ON CACHE BOOL "Radium uses assimp io")
set(RADIUM_IO_TINYPLY ON CACHE BOOL "Radium uses tinyply io")
~~~

When configuring Radium cmake project, don't forget to add this file by calling `cmake -C /path/to/installs/radium-external-r/radium-options.cmake  ...`

You can also provide these variables as cmake command line argument:

~~~{.bash}
cmake \
-DEigen3_DIR        /path/to/external/install/share/eigen3/cmake/
... and so on ...
~~~

# User provided external dependencies

You can  use your own installation of a local dependency instead of letting cmake fetch and compile it.
To this end, just provide the corresponding '*_DIR' to cmake at configuration time as show before (with '-D' option, configuration file or toolchain file, please refer to cmake documentation).

Currently supported (note that these paths must refer to the installation directory of the corresponding library):
<!--  (generated running ../script/list_dep.py from Radium-Engine/external directory) -->

* `stduuid_DIR`
* `RadiumNodeEditor_DIR`
* `assimp_DIR`
* `tinyply_DIR`
* `PowerSlider_DIR`
* `glm_DIR`
* `glbinding_DIR`
* `globjects_DIR`
* `stb_DIR`
* `tinyEXR_DIR`
* `Eigen3_DIR`
* `OpenMesh_DIR`
* `cpplocate_DIR`
* `nlohmann_json_DIR`

Radium is compiled and tested with specific version of dependencies, as given in the external's folder CMakeLists.txt and state here for the record

* stduuid: https://github.com/mariusbancila/stduuid, [3afe7193facd5d674de709fccc44d5055e144d7a],
  * with options `-DUUID_BUILD_TESTS=OFF -DUUID_ENABLE_INSTALL=ON`
* RadiumNodeEditor: https://github.com/MathiasPaulin/RadiumQtNodeEditor.git, [main],
  * with options `None`
* assimp: https://github.com/assimp/assimp.git, [tags/v5.0.1],
  * with options `-DASSIMP_BUILD_ASSIMP_TOOLS=False -DASSIMP_BUILD_SAMPLES=False -DASSIMP_BUILD_TESTS=False -DIGNORE_GIT_HASH=True -DASSIMP_NO_EXPORT=True`
* tinyply: https://github.com/ddiakopoulos/tinyply.git, [tags/2.3.2],
  * with options `-DSHARED_LIB=TRUE`
* PowerSlider: https://github.com/dlyr/PowerSlider.git, [origin/master],
  * with options `-DBUILD_DESIGNER_PLUGIN=OFF -DBUILD_EXAMPLE_APP=OFF`
* glm: https://github.com/g-truc/glm.git, [cc98465e3508535ba8c7f6208df934c156a018dc],
  * with options `-DGLM_TEST_ENABLE=OFF -DBUILD_STATIC_LIBS=OFF -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>`
* glbinding: https://github.com/cginternals/glbinding.git, [758d33b4d4f1815d46d91fcf4ca4914d837267fa],
  * with options `-DOPTION_BUILD_TESTS=OFF -DOPTION_BUILD_DOCS=OFF -DOPTION_BUILD_TOOLS=OFF -DOPTION_BUILD_EXAMPLES=OFF -DOPTION_BUILD_OWN_KHR_HEADERS=ON -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>`
* globjects: https://github.com/cginternals/globjects.git, [4363356ae2ef5c936ab078fba48d1ea507c295c4],
  * with options `-DOPTION_BUILD_DOCS=OFF -DOPTION_BUILD_EXAMPLES=OFF -DOPTION_USE_EIGEN=ON -Dglbinding_DIR=${glbinding_DIR} -Dglm_DIR=${glm_DIR} -DEigen3_DIR=${Eigen3_DIR} -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>`
* stb: https://github.com/nothings/stb.git, [1034f5e5c4809ea0a7f4387e0cd37c5184de3cdd],
  * with options `None`
* tinyEXR: https://github.com/MathiasPaulin/tinyexr.git, [origin/radium],
  * with options `-DBUILD_INSTALLABLE_LIB=ON`
* Eigen3: https://gitlab.com/libeigen/eigen.git, [tags/3.4.0],
  * with options `-DEIGEN_TEST_CXX11=OFF -DBUILD_TESTING=OFF -DEIGEN_BUILD_DOC=OFF`
* OpenMesh: https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git, [tags/OpenMesh-8.1],
  * with options `-DBUILD_APPS=OFF`
* cpplocate: https://github.com/cginternals/cpplocate.git, [tags/v2.2.0],
  * with options `-DOPTION_BUILD_TESTS=OFF -DOPTION_BUILD_DOCS=OFF`
* nlohmann_json: https://github.com/nlohmann/json.git, [tags/v3.10.5],
  * with options `-DJSON_Install=ON -DJSON_BuildTests=OFF`

<!--  (end script copy) -->

\warning You have to take care of the consistency of the external dependencies, e.g. it's not possible to use your version of globjects without providing your version of eigen, otherwise you will have mixed version in Radium.
