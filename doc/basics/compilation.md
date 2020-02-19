\page basicsCompileRadium Compilation instructions
[TOC]

## Dependencies
All the dependencies are automatically fetched and compiled at build time.
 * [Core] Eigen, OpenMesh
 * [Engine] glm, globjects, glbindings
 * [IO] Assimp
 * [GuiBase] Qt Core, Qt Widgets and Qt OpenGL v5.5+ (5.10 recommended)
 * stb_image

See @ref develbuildchain for technical details on dependencies management.

Minimal requirements
 * OpenGL 3+ / GLSL 330+
 * CMake 3.0.+

## Supported compiler and platforms

The following platforms and tool chains have been tested and should work :

 * *Windows* : MSVC 2017 cmake support, MinGW-32 4.9.2 (with Qt Creator).
 * *Mac OSX* : gcc 7.1 or higher, Apple clang
 * *Linux* : gcc 7.1  or higher, clang

### Continuous Integration:
 * *Linux (clang 5, gcc7) and Mac OSX* : https://travis-ci.org/STORM-IRIT/Radium-Engine
 * *Windows* (MSVC 2017): https://ci.appveyor.com/project/nmellado/radium-engine

## Build instructions

### Getting dependencies
They are automatically fetched by cmake at compile time.
To force cmake re-fetching the dependencies, called
~~~bash
make update
~~~

### Folder structure
<!--Radium-Engine relies on CMake buildchain on all supported platforms.
In most cases, building should be pretty straightforward, provided that cmake can locate the dependencies.
You will need to have the openGL headers and libraries, Qt 5.4 or more and cmake.
If cmake doesn't locate the Qt files (e.g. if you manually installed Qt as opposed to using your distribution's package),
see the troubleshooting section below.

See plateform-dependent instructions for detailled how-to.

Build output is generated in the `Radium-Engine/Bundle-*` directory (with `*` the name of the CXX compiler), with the following structure:
~~~
Bundle-*
  - 3rdPartyLibraries
  - Debug: bin/, lib/
  - Release: bin/, lib/
  - ...  #other build types if any (generated at compile time)
~~~
`3rdPartyLibraries` are always compiled in `Release` mode.
Plugins are generated in `bin/Plugins`.

TODO: Update this part of the documentation-->

### Configure build

Radium offers the following build option (off by default) :
* `RADIUM_WITH_DOUBLE_PRECISION` sets the floating point format to double-precision instead of single precisition

###  Building on Linux/MacOS (command line instruction)

Out-of source builds are mandatory, we recommand to follow the usual sequence:

~~~bash
$ mkdir build
$ cd build
$ cmake ..
$ make
~~~

Finally, the install target will copy all the radium related library in the same place, usefull for App compilation 

~~~bash
$ make install
~~~

All radium related cmake options (with their current values) can be printed with `cmake -LAH | grep -B1 RADIUM` (on linux like system)

~~~bash
// Enable testing. Tests are automatically built with target all
RADIUM_ENABLE_TESTING:BOOL=ON
--
// Include Radium::Core in CMake project
RADIUM_GENERATE_LIB_CORE:BOOL=ON
--
// Include Radium::Engine in CMake project
RADIUM_GENERATE_LIB_ENGINE:BOOL=ON
--
// Include Radium::GuiBase in CMake project
RADIUM_GENERATE_LIB_GUIBASE:BOOL=ON
--
// Include Radium::IO in CMake project
RADIUM_GENERATE_LIB_IO:BOOL=ON
--
// Include Radium::PluginBase in CMake project
RADIUM_GENERATE_LIB_PLUGINBASE:BOOL=ON
--
// Check submodules during build (will be automatically disabled after run)
RADIUM_GIT_UPDATE_SUBMODULE:BOOL=ON
--
// Install documentation. If RadiumDoc is compiled, install documentation to bundle directory for install target
RADIUM_INSTALL_DOC:BOOL=ON
--
// Provide loaders based on Assimp library
RADIUM_IO_ASSIMP:BOOL=OFF
--
// Provide depricated loaders (to be removed without notice)
RADIUM_IO_DEPRECATED:BOOL=ON
--
// Provide loaders based on TinyPly library
RADIUM_IO_TINYPLY:BOOL=ON
--
// [addExternalFolder] Skip updating Core::external (disable for rebuild)
RADIUM_SKIP_CORE_EXTERNAL:BOOL=ON
--
// [addExternalFolder] Skip updating Engine::external (disable for rebuild)
RADIUM_SKIP_ENGINE_EXTERNAL:BOOL=ON
--
// [addExternalFolder] Skip updating IO::external (disable for rebuild)
RADIUM_SKIP_IO_EXTERNAL:BOOL=ON
--
// Update version file each time the project is compiled (update compilation time in version.cpp)
RADIUM_UPDATE_VERSION:BOOL=ON
~~~


### Building on Microsoft Windows with Visual Studio

#### Supported versions of MSVC
Since Radium requires:
* the C++11/C++14/C++17 advanced features such as `constexpr`,
* cmake built-in support

you will need a recent MSVC (2017 minimum).
We tested our code with *VS 2017 Community* (https://www.visualstudio.com/products/visual-studio-community-vs), with the *CMake Tools for Visual Studio* extension.

See general instruction on cmake for Visual Studio here: https://blogs.msdn.microsoft.com/vcblog/2016/10/05/cmake-support-in-visual-studio/

#### Dependencies

*Qt* distributes version 5.10 with precompiled libraries for VS 2017 - 64 bits.
If using earlier versions of Qt (5.5)  or a different toolset you may have to compile Qt yourself.
You will probaby have to manually point cmake to the Qt folder (see Troubleshooting below)

Other dependencies (Eigen, Assimp, glbinding, globjects and glm) are included as a submodule in the git repository.

#### Getting started with Visual Studio

Thanks to the integrated support of CMake in Visual Studio, you don't need a VS solution to build your project: open the Radium folder (via *File* > *Open* > *Folder ...* or `devenv.exe <foldername>`).
VS should run cmake, generate the target builds (Debug and Release by default).
Other build types can be added by editing `CMakeSettings.json`.

You may have Cmake errors occuring at the first run (see Troubleshooting section below).
To fix them, you need to edit the VS-specific file `CMakeSettings.json`, via *CMake* > *Change CMake Settings* > path-to-CMakeLists (configuration-name) from the main menu.
For instance, it usually requires to set cmake build types manually, and to give path to Qt libraries.
To fix it, edit `CMakeSettings.json`, such that
~~~json
    {
      "name": "x64-Debug",
      "generator": "Ninja",
      "configurationType": "Debug",
      "inheritEnvironments": [ "msvc_x64_x64" ],
      "buildRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\build\\${name}",
      "installRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\install\\${name}",
      "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=C:/Qt-5.10/5.10.0/msvc2017_64 -DCMAKE_BUILD_TYPE=Debug",
      "buildCommandArgs": "-v",
      "ctestCommandArgs": ""
    },
    {
      "name": "x64-Release",
      "generator": "Ninja",
      "configurationType": "Release",
      "inheritEnvironments": [ "msvc_x64_x64" ],
      "buildRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\build\\${name}",
      "installRoot": "${env.USERPROFILE}\\CMakeBuilds\\${workspaceHash}\\install\\${name}",
      "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=C:/Qt-5.10/5.10.0/msvc2017_64 -DCMAKE_BUILD_TYPE=Release",
      "buildCommandArgs": "-v",
      "ctestCommandArgs": ""
    },
~~~
*Note*: it is strongly encouraged to use `/` separators in your path, instead of `\\` as previously mentionned. See https://stackoverflow.com/questions/13737370/cmake-error-invalid-escape-sequence-u

#### Compilation

Right click on CMakeList.txt > build > all.

### Building with QtCreator

QtCreator is supported on Windows, MacOS and Linux.
No specific requirement here, just open Radium-Engine CMake project and enjoy !
