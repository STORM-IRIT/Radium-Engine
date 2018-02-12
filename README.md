# Radium-Engine
Coolest engine ever made #yolo #swag

See [this presentation](https://docs.google.com/presentation/d/12W2KXY7ctJXFIelmgNEn7obiBv_E4bmcMl3mXeJPVgc/edit?usp=sharing)
for an overview of the project.

## Dependencies
* Eigen 3.3 (as submodule in repository)
* Assimp 3.2 (as submodule in repository)
* glbinding (as submodule in repository)
* globjects (as submodule in repository)
* glm (as submodule in repository)
* OpenGL 3+ / GLSL 330+
* Qt Core, Qt Widgets and Qt OpenGL v5.5+ (5.10 recommended)
* stb_image (in repository)
* To build : CMake 3.0.+

## Supported compiler and platforms

The following platforms and tool chains have been tested and should work :

* *Windows* : MSVC 2017 cmake support, MinGW-32 4.9.2 (with Qt Creator).
* *Mac OSX* : gcc 5 or higher, Apple clang
* *Linux* : gcc 5 or higher, clang

### Continuous Integration:
* *Linux (clang 5, gcc7) and Mac OSX* : https://travis-ci.org/STORM-IRIT/Radium-Engine
* *Windows* (MSVC 2017): https://ci.appveyor.com/project/nmellado/radium-engine

## Build instructions

### Getting submodules
Eigen, Assimp, glbinding, globjects, glm are submodules : you can get them by running these two commands
```bash
$ git submodule init
$ git submodule update
```
### Folder structure
Radium-Engine relies on CMake buildchain on all supported platforms.
In most cases, building should be pretty straightforward, provided that cmake can locate the dependencies.
You will need to have the openGL headers and libraries, Qt 5.4 or more and cmake.
If cmake doesn't locate the Qt files (e.g. if you manually installed Qt as opposed to using your distribution's package),
see the troubleshooting section below.

See plateform-dependent instructions for detailled how-to.

Build output is generated in the `Radium-Engine/Bundle-*` directory (with `*` the name of the CXX compiler), with the following structure:
```
Bundle-*
  - 3rdPartyLibraries
  - Debug: bin/, lib/
  - Release: bin/, lib/
  - ...  #other build types if any (generated at compile time)
```
`3rdPartyLibraries` are always compiled in `Release` mode.
Plugins are generated in `bin/Plugins`.

### Configure build

Radium offers two build options which are off by default :
* `RADIUM_WITH_DOUBLE_PRECISION` sets the floating point format to double-precision instead of single precisition
* `RADIUM_WITH_OMP` instructs the compiler to use OpenMP

###  Building on Linux/MacOS (command line instruction)

Out-of source builds are mandatory, we recommand to follow the usual sequence:

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

### Building on Microsoft Windows with Visual Studio

#### Supported versions of MSVC
Since Radium requires:
* the C++11/C++14 advanced features such as `constexpr`,
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
```json
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
```
*Note*: it is strongly encouraged to use `/` separators in your path, instead of `\\` as previously mentionned. See https://stackoverflow.com/questions/13737370/cmake-error-invalid-escape-sequence-u

#### Compilation

Right click on CMakeList.txt > build > all.

### Building with QtCreator

QtCreator is supported on Windows, MacOS and Linux.
No specific requirement here, just open Radium-Engine CMake project and enjoy !

## Troubleshooting

### Qt cmake errors
In case you run into an error like
```
By not providing "FindQt5Widgets.cmake" in CMAKE_MODULE_PATH this project
has asked CMake to find a package configuration file provided by
"Qt5Widgets", but CMake did not find one.
```
you need to set `CMAKE_PREFIX_PATH`, pointing to the Qt root dir of your commpiler.
For example on linux with gcc :
```bash
$ cmake -DCMAKE_PREFIX_PATH=/opt/Qt/5.x/gcc_64
```

On windows, using cmake-gui you can use the "add entry" button, adding `CMAKE_PREFIX_PATH`
as a string to point to the Qt directory (for example in the default installation :
`C:/Qt/5.6/msvc2015_64` )

### Crash when starting main application on windows
This is usally caused by missing dlls.
With Visual Studio, you may need to copy the Qt dlls to Radium bin folder `Bundle-MSVC\{Release-or-Debug}\bin`.

## Documentation
For more documentation about the engine (how to develop a plugin,
how renderer works, how to setup a scene file, ...), please refer to the Docs/ folder.
