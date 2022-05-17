\page basicsmanual Radium Basics
[TOC]

This part of the documentation describes how to compile Radium and use it in your own project.

TL;DR; (linux command line version)

```bash
git clone --recurse-submodules https://github.com/STORM-IRIT/Radium-Engine.git
mkdir radium-external # outside Radium Source dir
cmake Radium-Engine/external -DCMAKE_BUILD_TYPE=Release -B radium-external/build-r/ -DCMAKE_INSTALL_PREFIX=`pwd`/radium-external/install-r
cmake --build radium-external/build-r --parallel

cd Radium-Engine # go in Radium-Engine root dir
cmake -DCMAKE_BUILD_TYPE=Release -B build-r/ -C ../radium-external/install-r/radium-options.cmake  # or wherever you install radium externals
cmake --build build-r --parallel --target install

```
Radium-Engine installed files are in `Bundle-GNU`


More details and other systems information are in the following pages, to be read in order.
 * \subpage dependenciesmanagement : Fetch and compile dependencies
 * \subpage basicsCompileRadium : Compile and install Radium
 * \subpage basicsRadiumSubmodule : Link to Radium using cmake package
 * \subpage basicsTroubleshooting : Handle usual errors
