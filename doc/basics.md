\page basicsmanual Radium Basics
[TOC]

This part of the documentation describes how to compile Radium and use it in your own project.

## Supported system and compilers

The following platforms and tool chains have been tested and should work :

* CMake 3.18+
* *Windows* : IDEs: Visual Studio 2019, 2022 (2017 is not supported due to embedded cmake version), QtCreator. Command Line: cmake+ninja+MSVC(2017, 2019, 2022) .
* *Mac OSX* : gcc 10 or higher, Apple clang, llvm clang 11 or higher
* *Linux* : gcc 8  or higher, clang

See also our Continuous Integration system at <https://github.com/STORM-IRIT/Radium-Engine/actions>.

## System wide dependencies

Radium expects the following dependencies to be available during compilation.

* [Engine]
  * OpenGL 4.1+ / GLSL 410+
* [Gui]
  * Qt Core/Widgets/OpenGL 5.15 or 6.2+
* [Headless] One, or both of
  * EGL
  * glfw 3.3

## External dependencies (included with radium)

@RADIUM_DEPENDENCIES_VERSION@

## TL;DR; command line version

```bash
git clone --recurse-submodules https://github.com/STORM-IRIT/Radium-Engine.git

# optionally switch to a specific branch or tag, e.g. release-candidate or v1.2.0
# git switch release-candidate # use git checkout for tags
# git submodule update --init --recursive # in case submodules have changed wrt master

# configure and build (install automatically) external, outside Radium-Engine directory
cmake -S Radium-Engine/external -B builds/radium-external-build-r -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=installs/radium-external-r
cmake --build builds/radium-external-build-r --config Release --parallel

#configure build and install radium-engine
cmake -S Radium-Engine -B builds/radium-build-r -DCMAKE_BUILD_TYPE=Release -C installs/radium-external-r/radium-options.cmake
cmake --build builds/radium-build-r --config Release --parallel
cmake --install builds/radium-build-r
```

default Radium-Engine install prefix is `Radium-Engine/Bundle-${CMAKE_CXX_COMPILER_ID}`.

\note These commands might need some custom search directory for cmake config, e.g. for Qt, notably on windows. Please check detailed instructions below. Also several options are available, e.g. to compile examples or use double instead of float as scalar type.

## Detailed instructions

More details and other systems information (including visual studio setup) are in the following pages, to be read in order.

* \subpage basicsCompileVs
* \subpage basicsCompileCommand
* \subpage basicsRadiumSubmodule : Link to Radium using cmake package
* \subpage basicsCompileRadiumDetails
* \subpage basicsTroubleshooting : Handle usual errors
