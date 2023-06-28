\page basicsCompileRadium Radium Compilation instructions
[TOC]

# Supported compiler and platforms

The following platforms and tool chains have been tested and should work :

* *Windows* : IDEs: Visual Studio 2019, 2022 (2017 is not supported due to embedded cmake version), QtCreator. Command Line: cmake+ninja+MSVC(2017, 2019, 2022) .
* *Mac OSX* : gcc 10 or higher, Apple clang, llvm clang 11 or higher
* *Linux* : gcc 8  or higher, clang

See also our Continuous Integration system at <https://github.com/STORM-IRIT/Radium-Engine/actions>.

Minimal requirements

* OpenGL 4.1+ / GLSL 410+
* CMake 3.18+
* Qt5 (minimal version 5.15) or Qt6 (experimental)

# Build instructions

If not already done, follow instructions at \ref dependenciesmanagement.

Radium follows a standard cmake structure, so any IDE supporting cmake should be able to configure and build it.

Several targets allowing to build or install only a specific part of Radium-Engine are defined by the cmake configuration.
The standard targets `all` and `install` are also available as a shortcut to build all the configured components and to install the main components.

\note We strongly recommend to have dedicated build and install directories for each build type (Release, Debug).
Remember that compiling Radium in Debug mode needs to have the dependencies compiled and installed in Debug mode
(due to a limitation in assimp).

## Folder structure

Radium-Engine relies on CMake build-chain on all supported platforms.
In most cases, building should be pretty straightforward, provided that cmake can locate the dependencies.

### Installation directory

By default, `${CMAKE_INSTALL_PREFIX}` is set as follow:

* For release build :

~~~{.cmake}
    set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID})
~~~

* For Debug or RelWithDebInfo build

~~~{.cmake}
    set(RADIUM_BUNDLE_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Bundle-${CMAKE_CXX_COMPILER_ID}-${CMAKE_BUILD_TYPE})
~~~

It has the following structure

~~~{.txt}
Bundle-*
 - bin/  include/  lib/  LICENSE  README.md  Resources/
~~~

### Configure build options

Radium offers the following build options:

~~~{.bash}
// Enable coverage, gcc only. Experimental, need ENABLE_TESTING
RADIUM_ENABLE_COVERAGE:BOOL=OFF

// Enable examples app build. To install examples, build explicitly the target Install_RadiumExamples.
RADIUM_ENABLE_EXAMPLES:BOOL=OFF

// Enable precompiled headers.
RADIUM_ENABLE_PCH:BOOL=OFF

// Enable testing. Tests are automatically built with target all, run with target check or test.
RADIUM_ENABLE_TESTING:BOOL=ON

// Enable testing of OpenGL functionalities. Option only available if RADIUM_ENABLE_TESTING is ON.
RADIUM_ENABLE_GL_TESTING:BOOL=OFF

// Include Radium::Core in CMake project.
RADIUM_GENERATE_LIB_CORE:BOOL=ON

// Include Radium::Engine in CMake project.
RADIUM_GENERATE_LIB_ENGINE:BOOL=ON

// Include Radium::Gui in CMake project.
RADIUM_GENERATE_LIB_GUI:BOOL=ON

// Include Radium::Headless in CMake project.
RADIUM_GENERATE_LIB_HEADLESS:BOOL=ON

// Include Radium::IO in CMake project.
RADIUM_GENERATE_LIB_IO:BOOL=ON

// Include Radium::PluginBase in CMake project.
RADIUM_GENERATE_LIB_PLUGINBASE:BOOL=ON

// Install documentation. If RadiumDoc is compiled, install documentation to bundle directory for install target.
RADIUM_INSTALL_DOC:BOOL=ON

// Provide loaders based on Assimp library
RADIUM_IO_ASSIMP:BOOL=ON

// Provide deprecated loaders (to be removed without notice)
RADIUM_IO_DEPRECATED:BOOL=ON

// Provide loaders based on TinyPly library
RADIUM_IO_TINYPLY:BOOL=ON

// Provide loader for volume pvm file format
RADIUM_IO_VOLUMES:BOOL=ON

// Disable Radium Log messages
RADIUM_QUIET:BOOL=OFF

// Update version file each time the project is compiled (update compilation time in version.cpp).
RADIUM_UPDATE_VERSION:BOOL=ON

// Use double precision for Scalar.
RADIUM_USE_DOUBLE:BOOL=OFF
~~~

All radium related cmake options (with their current values) can be printed with `cmake -LAH | grep -B1 RADIUM` (on linux like system)

\warning For computers with low RAM capacities (under 12G) we recommend to set the `CMAKE_BUILD_PARALLEL_LEVEL` environment variable to a reasonable value (i.e. 2) to prevent the computer from swapping.

### Precompiled headers

Radium build can take benefit of precompiled headers. To do so, set `RADIUM_ENABLE_PCH=ON` (OFF by default) at cmake configure time.
Compilation speedup depends on what files you are currently editing, and full compilation is roughly 20% less with precompiled header enabled.

### Dependencies between libraries

The options `RADIUM_GENERATE_LIB_XXXX` allows to enable/disable each Radium library.
The dependencies between libraries are set as follow:

~~~{.cmake}
add_dependencies (${ra_engine_target} PUBLIC Radium::Core)
add_dependencies (${ra_io_target} PUBLIC Radium::Core)
add_dependencies (${ra_pluginbase_target} Radium::Core Radium::Engine)
add_dependencies (${ra_gui_target} PUBLIC Radium::Core Radium::Engine Radium::PluginBase Radium::IO)
~~~

\warning Consistency of `RADIUM_GENERATE_LIB_***` options is not checked wrt. the dependencies.

* When enabled using `RADIUM_GENERATE_LIB_***`, each library has a compilation target: `Core`, `Engine`, ...

## Command line instructions for building (on windows, mac and linux)

Out-of source builds are mandatory, we recommend to follow the usual sequence, assuming you have build dependecies as explained [here](@ref builddep):

~~~{.bash}
cmake -S Radium-Engine -B builds/radium-build-r -DCMAKE_BUILD_TYPE=Release -C installs/radium-external-r/radium-options.cmake
cmake --build builds/radium-build-r --config Release --parallel -DQt5_DIR=path/to/qt5
cmake --install builds/radium-build-r

~~~

If qt is installed system wide (likely on linux), `-DQt5_DIR` is not needed.

\note Qt6 is also supported. To enable it, replace `-DQt5_DIR=path/to/qt5` by `-DQt6_DIR=path/to/qt6`. To ease maintenance accross Qt versions, you should also configure the path to Qt cmake package using `-DCMAKE_PREFIX_PATH=path/to/qtX` where `X` is the Qt version you want to use.

If both Qt5 and Qt6 are installed system wide, Qt6 is the default, `-DQT_DEFAULT_MAJOR_VERSION=5` allow select Qt5. During client application cmake setup `find_package(Radium COMPONENTS ... Gui ...)` will check Qt version consistency.

\note Running `cmake --install` is recommended as it will copy all the radium related library in the same place,
generate the cmake packages and bundle applications with their dependencies (on macos and windows).

## Integration with Visual Studio (Microsoft Windows)

### Supported versions of MSVC

Radium requires MSVC 2019 or superior, as it relies on:

* C++11/C++14/C++17 features such as `constexpr`,
* cmake built-in support
* Ninja built-in support

Our Continuous Integration systems uses Microsoft Compiler 2017, in combination with cmake and ninja.
Using Visual Studio 2017 with cmake support is however not possible: VS is shipped with cmake: 3.12, while Radium requires cmake 3.13 at least. We recommend to use Visual Studio 2019 in that case.
Qt 5.15+ is distributed with binaries precompiled with MSVC 2019, but Qt binaries precompiled with MSVC2017 does not break the build.

### Qt Dependency

Use precompiled libraries for VS 2017 or 2019 - 64 bits (minimal version required: 5.14).
You will probably have to manually point cmake to the Qt folder.

### Getting started with Visual Studio

Thanks to the integrated support of CMake in Visual Studio, you don't need a VS solution to build your project: open the Radium folder (via *File* > *Open* > *Folder ...* or `devenv.exe <foldername>`).
VS should run cmake, generate the target builds (Debug and Release by default).
Other build types can be added by editing `CMakeSettings.json`.

Configure cmake option (see official doc [here](https://docs.microsoft.com/cpp/build/customize-cmake-settings))
You have to provide path to Qt installation, glfw installation (for headless support) and external dependencies configuration.
In order to execute Radium demos from the buildtree (installing them takes time due to qt deployement procedure),
you also need to define environment variables that point to the various dlls used by Radium components that will be used on the per-target `launch.vs.json` configuration file.
Note that it is recommended to compile or install glfw as a static library, the example below assume that.

For instance, with directory structure for externals as defined in \ref dependenciesmanagement, the configuration is

~~~{.json}
{
    "environments": [
    {
        "QtDir": "C:/Qt/6.3.0/msvc2019_64/"
        "glfwDir" : "C:/path/to/glfwInstallation",
        "ExternalInstallDir": "${projectDir}/../radium-externals/install;${projectDir}/../radium-externals/install/${name}/cpplocate;"
    }
    ],
    "configurations": [
    {
        "name": "x64-Debug",
        "generator": "Ninja",
        "configurationType": "Debug",
        "inheritEnvironments": [ "msvc_x64_x64" ],
        "buildRoot": "${projectDir}/out/build/${name}",
        "installRoot": "${projectDir}/out/install/${name}",
        "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=\"${env.QtDir};${env.glfwDir}/lib/cmake\" -C ${projectDir}/../radium-externals/install/${name}/radium-options.cmake",
        "buildCommandArgs": "",
        "ctestCommandArgs": "",
        "environments": [
        {
            "environment": "RadiumDllsLocations",
            "ExternalDllsDIR": "${env.ExternalInstallDir}/${name}/bin;${env.ExternalInstallDir}/${name}/cpplocate;${env.ExternalInstallDir}/${name}/glbinding;${env.ExternalInstallDir}/${name}/globjects",
            "QtDllsDIR": "${env.QtDir}/bin",
            "RadiumDlls": "${buildRoot}/src/Core;${buildRoot}/src/Engine;${buildRoot}/src/Gui;${buildRoot}/src/Headless;${buildRoot}/src/IO;${buildRoot}/src/PluginBase"
        }
        ]
    },
    {
        "name": "x64-Release",
        "generator": "Ninja",
        "configurationType": "Release",
        "inheritEnvironments": [ "msvc_x64_x64" ],
        "buildRoot": "${projectDir}/out/build/${name}",
        "installRoot": "${projectDir}/out/install/${name}",
        "cmakeCommandArgs": "-DCMAKE_PREFIX_PATH=\"${env.QtDir};${env.glfwDir}/lib/cmake\" -C ${projectDir}/../radium-externals/install/${name}/radium-options.cmake",
        "buildCommandArgs": "",
        "ctestCommandArgs": "",
        "environments": [
        {
            "environment": "RadiumDllsLocations",
            "ExternalDllsDIR": "${env.ExternalInstallDir}/${name}/bin;${env.ExternalInstallDir}/${name}/cpplocate;${env.ExternalInstallDir}/${name}/glbinding;${env.ExternalInstallDir}/${name}/globjects",
            "QtDllsDIR": "${env.QtDir}/bin",
            "RadiumDlls": "${buildRoot}/src/Core;${buildRoot}/src/Engine;${buildRoot}/src/Gui;${buildRoot}/src/Headless;${buildRoot}/src/IO;${buildRoot}/src/PluginBase"
        }
        ]
    }
    ]
}
~~~

\note It is strongly encouraged to use `/` separators in your path, instead of `\\`. See <https://stackoverflow.com/questions/13737370/cmake-error-invalid-escape-sequence-u>

\note When compiling the dependencies you may hit the max path length fixed by Microsoft OS (260 characters). To fix this, you might need to change the path of your build dir to shorten it, or to change the limit on your system, see: <https://docs.microsoft.com/en-us/windows/win32/fileio/naming-a-file#enable-long-paths-in-windows-10-version-1607-and-later>

### Compilation

Right click on `CMakeList.txt > build > all`.

### Execution of a demo app

To execute a demo application, select in the `Startup Item` list the target you want to execute.
For instance, `DrawPrimitives.exe (examples/...)`.
Then, select in the menu `<Debug>/Debug and Launch Settings for DrawPrimmitives` and modify the `launch.vs.json` file that is opened so that it contains the following.

~~~{.json}
{
 "version": "0.2.1",
 "defaults": {},
 "configurations": [
   {
     "type": "default",
     "project": "CMakeLists.txt",
     "projectTarget": "DrawPrimitives.exe (examples\\DrawPrimitives\\DrawPrimitives.exe)",
     "name": "DrawPrimitives.exe (examples\\DrawPrimitives\\DrawPrimitives.exe)",
     "inheritEnvironments": [ "RadiumDllLocations" ],
     "env": {
       "PATH": "${env.QtDllsDIR};${env.ExternalDllsDIR};${env.RadiumDlls};${env.PATH}"
     }
   }
 ]
}
~~~

For any target you want to execute, the same should be done, i.e. adding the following to the target configuration

~~~{.json}
 "inheritEnvironments": [ "RadiumDllLocations" ],
 "env": {
   "PATH": "${env.QtDllsDIR};${env.ExternalDllsDIR};${env.RadiumDlls};${env.PATH}"
 }
~~~

If you plan to execute the 'unittest.exe' target, you should also add the following configuration for the working directory

~~~{.json}
 "currentDir": "${projectDir}/tests/unittest"
~~~

### installation

To install, you need to run any installation target, e.g. `Engine.dll (install)` or to select the menu `<Build>/<Install radiumproject>`
