\page basicsmanual Radium Basics
[TOC]

This part of the documentation describes how to compile Radium and use it in your own project.

TL;DR; command line version.

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

* \subpage dependenciesmanagement : Fetch and compile dependencies
* \subpage basicsCompileRadium : Compile and install Radium
* \subpage basicsRadiumSubmodule : Link to Radium using cmake package
* \subpage basicsTroubleshooting : Handle usual errors
