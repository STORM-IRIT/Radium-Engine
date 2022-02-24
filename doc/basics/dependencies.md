\page dependenciesmanagement Radium Dependencies Management
[TOC]

Radium relies on several external libraries to load files or to represent some data:
* [Core] Eigen, OpenMesh
* [Engine] glm, globjects, glbindings
* [IO] Assimp
* [Gui] Qt Core, Qt Widgets and Qt OpenGL v5.5+ (5.14 at least, Qt6 support is experimental)
* [doc] Doxygen-awesome-css
* stb_image

We developed a series of tools to fetch and compile these dependencies easily, except for
Qt, which needs to be installed and passed to cmake through the variables `Qt5_DIR` OR `Qt6_DIR` (see documentation at
https://doc.qt.io/qt-5.15/cmake-manual.html#getting-started).

# Dependencies management systems

We offer three different systems to handle external dependencies (see details and how-to in the following sections):
 1. **[recommended]** external build: the user compiles and installs once for all the dependencies using a dedicated
    cmake project. Then, Radium cmake project is configured to link with installed dependencies.
 2. automatic build: the dependencies are automatically fetched and compiled by cmake when configuring Radium. This
    automated process is handy for Continuous Integration systems. For standard users, it slows down the configuration
    step of Radium and requires to fetch and recompile the dependencies each time the build directory is cleaned.
 3. manual management: users can also provide their own version of the dependencies through cmake packages.

\warning Dependencies need to be built with the same build type (ie. Release, Debug) than Radium.

# Building and installing Radium dependencies once for all

We provide a standalone cmake project (`external/CMakeLists.txt`) to compile and install the Radium dependencies at any location
_outside Radium-Engine source dir_.
\warning If you try to build and install external as a subdir of Radium-Engine source, cmake configuration will fail.

## Configuration and compilation of the dependencies
Create installation directory, configure and build the cmake project:
~~~{.bash}
mkdir /path/to/external/installDir/Release
mkdir BuildRadiumDependencies && cd BuildRadiumDependencies
cmake -DCMAKE_INSTALL_PREFIX=/path/to/external/installDir/Release /path/to/Radium-Engine/external -DCMAKE_BUILD_TYPE=Release
make .
~~~

If not given on the command line, the installation directory is set by default to `{CMAKE_CURRENT_BINARY_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE}`.
\warning The default installation directory will not work if `{CMAKE_CURRENT_BINARY_DIR}` is a subdirectory of the Radium folder.

Then, to compile Radium-Engine, you have to indicate where cmake can find each dependency, as detailed next.

## Configuration of Radium
### Through a configuration file
For convenience, we recommend to store your configuration in a file and use the `-C` option of cmake.

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

When configuring Radium cmake project, don't forget to add this file by calling `cmake -C /path/to/myconfig.cmake  ......`

### Manually using command line
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


# Fetch and build at configure time
When Radium is configured without specifying the paths to the dependencies, cmake automatically fetch, compile and
install them.
\note In this mode, dependencies are fetched and built at **configure** time:

~~~{.sh}
$ cmake .. -DRADIUM_GENERATE_LIB_IO=OFF -DRADIUM_GENERATE_LIB_ENGINE=OFF -DRADIUM_GENERATE_LIB_GUI=OFF -DRADIUM_GENERATE_LIB_PLUGINBASE=OFF -DRADIUM_ENABLE_TESTING=OFF
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
[addExternalFolder] Configure package file /****/build/src/Core/external/cmake/package-Core.cmake
Configure library Core with default settings
clang-tidy - static analysis              NO
cppcheck - static analysis                NO
clang-format - code formating             YES
Sanitizers:
  + ADDRESS_SANITIZER                     OFF
  + UB_SANITIZER                          OFF
  + THREAD_SANITIZER                      OFF
┌────────────────────┤│ Final overview for radiumproject │├─────────────────────┐
...........
└───────────────────────────────────────────────────────────────────────────────┘
-- Configuring done
-- Generating done
-- Build files have been written to: /****/build/
~~~

What is happening here, is that the dependencies of Core are fetched using git, then
configured using cmake, built, and installed to ${CMAKE_INSTALL_PREFIX} of Radium.
\see File `cmake/externalFunc.cmake` for technical details.

In order to save compilation time, dependencies are processed at the first run, and then skipped.
To force processing again the dependencies of a given library, turn on the associated option

~~~
OPTION( RADIUM_SKIP_${NAME_UPPER}_EXTERNAL "[addExternalFolder] Skip updating ${NAME}::external (disable for rebuild)" ON)
~~~

where `$NAME_UPPER` is `CORE`, `ENGINE`, `GUI`, `PLUGINBASE` or `IO`.
\note The option `RADIUM_SKIP_${NAME_UPPER}_EXTERNAL` is generated only if `RADIUM_GENERATE_LIB_${NAME_UPPER}` is `ON`.




# User provided external dependencies

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
