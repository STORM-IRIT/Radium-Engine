\page develbuildchain Radium Build Chain
[TOC]

### Installation directory
By default, `${CMAKE_INSTALL_PREFIX}` is set as follow:
~~~{.cmake}
set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID})
~~~

### Radium Libraries
#### Overview
Radium is split in 5 libraries: Core, Engine, GuiBase, UI and PluginBase.
   The compilation of each library is controlled by the following cmake options
~~~{.cmake}
option(RADIUM_GENERATE_LIB_CORE       "Include Radium::Core in CMake project" ON)
option(RADIUM_GENERATE_LIB_IO         "Include Radium::IO in CMake project" ON)
option(RADIUM_GENERATE_LIB_ENGINE     "Include Radium::Engine in CMake project" ON)
option(RADIUM_GENERATE_LIB_GUIBASE    "Include Radium::GuiBase in CMake project" ON)
option(RADIUM_GENERATE_LIB_PLUGINBASE "Include Radium::PluginBase in CMake project" ON)
~~~
#### Dependencies between libraries
~~~{.cmake}
add_dependencies (${ra_engine_target} PUBLIC Radium::Core)
add_dependencies (${ra_io_target} PUBLIC Radium::Core)
add_dependencies (${ra_pluginbase_target} Radium::Core Radium::Engine)
add_dependencies (${ra_guibase_target} PUBLIC Radium::Core Radium::Engine Radium::PluginBase Radium::IO)
~~~
\warning Consistency of `RADIUM_GENERATE_LIB_***` options is not checked wrt. the dependencies.

 - When enabled using `RADIUM_GENERATE_LIB_***`, each library has a compilation target: `Core`,
 `Engine`, ...

#### External dependencies
Each library comes with its own dependencies, which are fetched and built at **configure** time:
~~~{.sh}
$ cmake .. -DRADIUM_GENERATE_LIB_IO=OFF -DRADIUM_GENERATE_LIB_ENGINE=OFF -DRADIUM_GENERATE_LIB_GUIBASE=OFF -DRADIUM_GENERATE_LIB_PLUGINBASE=OFF -DRADIUM_ENABLE_TESTING=OFF
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
INFO[addExternalFolder] process Core /*****/Radium-Engine/src/Core/external
[addExternalFolder] Create temporary directory
[addExternalFolder] Configure cmake project
[addExternalFolder] Start build
[addExternalFolder] Build ended
[addExternalFolder] Configure package file /****/build-Radium-Engine-Desktop-Release/src/Core/external/cmake/package-Core.cmake
Configure library Core with default settings
clang-tidy - static analysis              NO
cppcheck - static analysis                NO
clang-format - code formating             YES
Sanitizers:
  + ADDRESS_SANITIZER                     OFF
  + UB_SANITIZER                          OFF
  + THREAD_SANITIZER                      OFF
    == Final overview for radiumproject ==
Version:               1.0.0 beta @ cezanne
Install prefix:        ****/Bundle-GNU
Compiler:              /usr/bin/g++
CMAKE_BUILD_TYPE:      Release
CMAKE_INSTALL_PREFIX:  ****/Bundle-GNU
RADIUM_ENABLE_TESTING: OFF
  possible options: Debug Release RelWithDebInfo MinSizeRel
  set with ` cmake -DCMAKE_BUILD_TYPE=Debug .. `
Configuring done
Generating done
CMake Project was parsed successfully.
~~~
Whats is happening here, is that the dependencies of Core are fetched using git, then
configured using cmake, built, and installed to ${CMAKE_INSTALL_PREFIX} of Radium.
\see File `cmake/externalFunc.cmake` for technical details.

In order to save compilation time, dependencies are processed at the first run, and then skipped.
To force processing again the dependencies of a given library, turn on the associated option
~~~
OPTION( RADIUM_SKIP_${NAME_UPPER}_EXTERNAL "[addExternalFolder] Skip updating ${NAME}::external (disable for rebuild)" ON)
~~~
where `$NAME_UPPER` is `CORE`, `ENGINE`, `GUIBASE`, `PLUGINBASE` or `IO`.
\note The option `RADIUM_SKIP_${NAME_UPPER}_EXTERNAL` is generated only if `RADIUM_GENERATE_LIB_${NAME_UPPER}` is `ON`.
