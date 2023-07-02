\page dependenciesmanagement Radium Dependencies Management Details
[TOC]

We developed a series of tools to fetch and compile these dependencies easily, except for
Qt, which needs to be installed and passed to cmake through the variables `CMAKE_PREFIX_PATH` or `Qt5_DIR` `Qt6_DIR`
(see documentation at <https://doc.qt.io/qt-5.15/cmake-manual.html#getting-started>).

# Dependencies management systems

We offer two different systems to handle external dependencies (see details and how-to in the following sections):

 1. **[recommended]** external build: the user compiles and installs once for all the dependencies using a dedicated
    cmake project. Then, Radium cmake project is configured to link with installed dependencies, as described in compilation instructions.
 2. manual management: users can also provide their own version of the dependencies through cmake packages.

\warning Dependencies need to be built with the same build type (ie. Release, Debug) than Radium.

## Configuration of Radium

To compile Radium-Engine, you have to indicate where cmake can find each dependency.

For convenience, dependencies install procedure create a configuration file, to be used with the `-C` option of cmake.
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
<!--  (generated running ../scripts/list_dep.py from Radium-Engine/external directory) -->

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

<!--  (end script copy) -->

\warning You have to take care of the consistency of the external dependencies, e.g. it's not possible to use your version of globjects without providing your version of eigen, otherwise you will have mixed version in Radium.
