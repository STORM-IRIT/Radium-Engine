\page basicsmanual Radium Basics
[TOC]

This part of the documentation describes how to compile Radium and use it in your own project.

TL;DR; (linux command line version)

```bash
git clone --recurse-submodules https://github.com/STORM-IRIT/Radium-Engine.git
cd Radium-Engine/external
cmake -DCMAKE_BUILD_TYPE=Release -B build-r/ . -DCMAKE_INSTALL_PREFIX=`pwd`/install-r
cmake --build build-r --parallel

cd .. # go back in Radium-Engine root dir
cmake -DCMAKE_BUILD_TYPE=Release -B build-r/ -C external/install-r/radium-options.cmake
cmake --build build-r --parallel --target install

```
Radium-Engine installed files are in `Bundle-GNU`


More details and other systems information are in the following pages, to be read in order.
 * \subpage dependenciesmanagement : Fetch and compile dependencies
 * \subpage basicsCompileRadium : Compile and install Radium
 * \subpage basicsRadiumSubmodule : Link to Radium using cmake package
 * \subpage basicsTroubleshooting : Handle usual errors
