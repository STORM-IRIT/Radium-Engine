\page cmakeutilities Cmake utilities for developing using Radium 
[TOC]

The Radium environment could be used in several ways 
 1. developing some specific applications, 
 2. extending the Radium capabilities by developing new libraries
 3. integrating new functionalities in the Radium applications using plugins.

As Radium rely on the [cmake](https://cmake.org/documentation/) build and configuration system, this 
documentation focus on how to configure your build system to extend Radium.

# Radium cmake package
Once installed, either from source or from pre-built binaries, Radium could be used in any application as a cmake package.
To integrate the Radium libraries into your build-chain configuration, you have to ask cmake to find the Radium package.

As for any cmake package, this could be done by adding the following line into your `CMakeLists.txt` file :

~~~{.cmake}
find_package(Radium REQUIRED Core Engine)
~~~
Remember to configure your project by giving the cmake command line option `-DRadium_DIR=/path/to/installed/Radium/lib/cmake/Radium` so that the `find_package` command could find the Radium package definition.
Once found, the Radium package defines a cmake target for each requested components (`Core`and `Engine` in the example above) in the cmake namespace `Radium`
When configuring your own target in your `CMakeLists.txt` file, you just need to link with those targets to get access to all the public interface of Radium (include search path, libraries, ...).

The radium package also defines several cmake functions, described below, that you can use to ease the configuration of your application, library or plugin, mainly to install them in a relocatable way while allowing their use from their own build-tree. 
Th functions defined by the Radium package are the following:
 - [configure_radium_library](#configure_radium_library).
 - [installTargetResources](#installTargetResources).
 - [configure_radium_package](#configurePackage)
 
## Function configure_radium_library {#configure_radium_library}
~~~{.cmake}
configure_radium_library( 
    TARGET targetName               # Name of the target to configure as a Radium Library
    FILES file1[file2[file3 ...]]   # list of headers to install
    [TARGET_DIR directoryName]      # Name of the directory where files are installed (default <prefix>/include/<TARGET>)
    [NAMESPACE NameSpace]           # Namespace of the imported target (default Radium)
    [PACKAGE_CONFIG file.cmake.in]  # name of the configure script model for the given target
    [PACKAGE_DIR packageDirName]    # Name of the directory where the cmake config file will be installed (default <prefix>/lib/cmake/Radium)
)
~~~
This cmake function configures the `<TARGET>` for installation and for further import in client project using `find_package(<TARGET>)`.
In order to use the library either through an imported target from an installed binary or as a project target in another component on the same build tree, this function defines an alias target with the same name than the imported one.
This function also defines the symbol <TARGET>_EXPORTS so that, it could be used to allow symbol import/export from dynamic library.

This function takes the following parameters :
 
  - `<TARGET>`. The name of the target to configure
  - `<FILES>`. Expected to be public headers, they will be installed in the include directory of the project installation configuration `${CMAKE_INSTALL_PREFIX}/include`.
  - `<TARGET_DIR>`. If given, `<FILES>` will be installed into the `${CMAKE_INSTALL_PREFIX}/include/<TARGET_DIR>` directory. If not, the files will be installed into `${CMAKE_INSTALL_PREFIX}/include/<TARGET>` directory.
  - `<NAMESPACE>`. If given, the imported target will be `<NAMESPACE>::<TARGET>`. If not, the imported target will be `Radium::<TARGET>`
  - `<PACKAGE_CONFIG>`. If given, a configure script, to be used by `find_package`, will be generated. If not, only the exported targets will be generated.
  - `<PACKAGE_DIR>`. If given, the cmake configuration script `<TARGET>Config.cmake` searched by `find_package(<TARGET>)` will be installed in the directory `${CMAKE_INSTALL_PREFIX}/<PACKAGE_DIR>`. If not, the configure script will be installed in the directory `<${CMAKE_INSTALL_PREFIX}/lib/cmake/Radium`.

In order to allow usage of installed library in all supported systems (Linux, MacOs, Windows), one of the public header, that might be named, e.g. `<TARGET>Macros.hpp` and included all the public or private headers, should contain the following :

~~~{.cpp}
#pragma once
#include <Core/RaCore.hpp>
#if defined <TARGET>_EXPORTS
#    define <TARGET>_API DLL_EXPORT
#else
#    define <TARGET>_API DLL_IMPORT
#endif
~~~

Then, in the library source code, each exported symbol should be prefixed by `<TARGET>_API` e.g.
~~~{.cpp}
#pragma once
#include < <TARGET>/<TARGET>Macros.hpp >
class <TARGET>_API MyAwesomeMaterial : public Ra::Engine::Material
{
    ...
};

<TARGET>_API bool initializeMyLibrary();
~~~

Assuming a library `MyLib` was configured as follows:
~~~{.cmake}
... 
configure_radium_library(
        TARGET MyLib
        NAMESPACE MyLib
        PACKAGE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake
        PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        FILES "${public_headers};${public_inlines}"
)
~~~

This library could be used either from its installed binaries :
~~~{.cmake}
find_package(MyLib) 
...
target_link_libraries(
        MyProject
        PUBLIC
        Radium::Core
        Radium::Engine
        MyLib::MyLib
)
~~~
or from the same buildtree :
~~~{.cmake}
add_subdirectory(MyLibSource) 
...
target_link_libraries(
        MyProject
        PUBLIC
        Radium::Core
        Radium::Engine
        MyLib::MyLib
)
~~~

## Function installTargetResources {#installTargetResources}
~~~{.cmake}
installTargetResources( 
    TARGET targetName               # Name of the target for which resources must be installed                      
    DIRECTORY theResourceDirectory  # location, in the source tree, of the resources 
    [BUILD_LOCATION whereToLink]    # In the build tree, where to link the resources (default ${CMAKE_CURRENT_BINARY_DIR}/../Resources
    [PREFIX TargetResourcePrefix]   # In the install tree, where to install resources (default <empty>).
    [FILES[file1[file2...]]]        # Resources individual files to install
~~~

When a Radium based component needs to access several resources to implement its functionalities, this function installs the required resources.
This function takes the following parameters :
 
  - `<TARGET>`. The name of the target to configure.
  - `<DIRECTORY>`. The directory in the source tree that contains the resource to install.
  - `<BUILD_LOCATION>`. If given, `<DIRECTORY>` will be linked in the build tree at this location. If not, the resource directory will be linked to `${CMAKE_CURRENT_BINARY_DIR}/../Resources` directory.
  - `<PREFIX>`. If given, the resources will be installed into the directory `<install_prefix>/Resources/<PREFIX>/`. If not, the resources will be installed into the directory `<install_prefix>/Resources/` where `<install_prefix>` is the installation directory of the target.
  - `<FILES>`. If given, this allow to install only the given files from `<DIRECTORY>`. If not, all the files from `<DIRECTORY>` will be installed.
 
For a Radium-based software component, resources can be of several types:
  - Text files, that contain some glsl source code used for shaders.
  - Images used internally as textures or source of data.
  - Precomputed data, stored in whatever format.
  
To access resources, the component might rely on a Ra::Core::Resources::ResourcesLocator object. Such an object allows to access files located in some predefined directories:
  - Radium default resource directory. This directory is located at `<prefix>/Resources` where `<prefix>` is the Radium installation directory.
  - Current executable directory. This directory is where the currently running executable is located.
  
In order to allow client component to access its own resources, and to allow that either on the build-tree or once the component is installed without having to recompile or modify anything, this function both links (on supported systems, copy elsewhere) and installs the component resources at the given places.

As the process must be system and component independent it is the responsibility of the client package to configure the proper paths for resources management.

According to the installation layout of the Radium lib, that matches the GNU standard, but also to the MacOs bundle architecture, Resources might be localized relatively to any binary object (library, executable, plugin, ...)
The Radium installation hierarchy, that embeds base libraries, their resources and some executable files is the following:
```
<prefix>/
├── include/
|   ├── Core/
|   ├── Engine/
|   ...
├── lib/
|   ├── cmake/
|   |   └── Radium/
|   ├──  libCore.so
|   ...
├── bin/
|   ├──  main-app
|   ...
├── Resources/
|   ├── Shaders/
|   ├── Config/
|   ...
```
In this hierarchy, Resources are located in the relative path `../Resources/` from any binary (executable or lib).
The source code structure of the Radium libraries allows to have the same property on the buildtree.
When a Radium component needs to access some resources, it could either use the Ra::Core::Resources::ResourcesLocator::getRadiumResourcesDir() method or search for the resource relatively to a symbol defined in the component.

To do this, the source code (.cpp file) of the component could contain the following.
~~~{.cpp}
#include <MyComponent.hpp>

static int MyComponentMagick = 0x0F0F0F0F;
...
// get the resource path relatively to the current location of the binary 
auto resourcesPath = Ra::Core::Resources::ResourcesLocator(
                    reinterpret_cast<void*>( &MyComponentMagick ), 
                    "/Resources/MyComponentResources/", 
                    "../" );
// Read files from the found resourcesPath that will prefix the Resource files or directory
~~~

When using the [installTargetResources](#installTargetResources) function to configure the component resources installation, the following call should be made to allow access from both the build tree and the install tree using the above snippet.

~~~{.cmake}
# Assuming MyComponentTarget was configured as a library, plugin or executable, this will configure the resource access in the build tree and the install tree
installTargetResources(
    TARGET MyComponentTarget                            # The name of the target
    DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/MyResource    # Where are the resources in the source tree
    BUILD_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/Resources/MyComponentResources # The resources will be linked here in the buildtree
    PREFIX MyComponentResources # the resources will be installed in <prefix>/Resources/MyComponentResources
)
~~~

## Function configure_radium_package {#configurePackage}
~~~{.cmake}
configure_radium_package( 
    NAME packageName                # The name of the package to install
    PACKAGE_CONFIG configFile.in    # The package configuration file
    [PACKAGE_DIR packageDirName]    # Name of the directory where the cmake config file will be installed (default <prefix>/lib/cmake/Radium)
)
~~~
This cmake function configures the package `packageName for installation and for further import in client project using `find_package(<TARGET>)`.`
This function allows to define multi-component packages for selective import using the `find_package(packageName [COMPONENTS comp1 comp2 ...]` command.

This function takes the following parameters :
 
  - `<NAME>`. The name of the package to configure and install
  - `<PACKAGE_CONFIG>`. The configure script to be used by `find_package`.
  - `<PACKAGE_DIR>`. If given, the cmake configuration script `<TARGET>Config.cmake` searched by `find_package(<TARGET>)` will be installed in the directory `${CMAKE_INSTALL_PREFIX}/<PACKAGE_DIR>`. If not, the configure script will be installed in the directory `<${CMAKE_INSTALL_PREFIX}/lib/cmake/Radium`.

# How to write your CMakeLists.txt
When writing your cmake configuration script `CMakeLists.txt`, you might rely on the following guideline to configure the project `ProjectName`.
Note that these are only guidelines and that you can always write your cmake script from scratch, assuming you understand what you do.

## General cmake preamble
~~~{.cmake}
# -------------------------------------------------------
# Recommended preamble for cmake configuration
# -------------------------------------------------------
# Radium package requires cmake minimum version 3.13 
cmake_minimum_required(3.13)
# It is recommended to disable in-source build
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
# if not explicitely set by the user, set the install prefix to a bundle directory in the build tree
if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/installed-${CMAKE_CXX_COMPILER_ID}" CACHE PATH
        "Install path prefix, prepended onto install directories." FORCE)
    message("Set install prefix to ${CMAKE_INSTALL_PREFIX}")
    set(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT False)
endif ()
# -------------------------------------------------------
# Configure the project
# -------------------------------------------------------
project(ProjectName VERSION 1.0.0)

# The project requires Core and Engine component from Radium
find_package(Radium REQUIRED Core Engine)

...
~~~

Then, according to the type of the targets defined in your project (application, single library, set of libraries, plugin, mix of them), you might configure your project according to the following guidelines.

## Configuring a single installable library
An installable library could be used to add functionalities built over the Radium libraries and to make these functionalities available to developers.
Once installed, a library consists in:
 - a set of public headers
 - a dynamic library (`.so`, `.dylib` or `.dll`)
 - a cmake config file so that the library could be fetched by the cmake command `find_package`
 - [optionally] a set of associated resources

In order to fulfill Radium conventions, a library will be identified by a name into a namespace.

To configure an installable library based on Radium, it is recommended to have the following `CMakeLists.txt`

~~~{.cmake}
set(NAME_OF_LIBRARY NameOfTheLibrary)
# Build the lists of files constituting the target
set(sources
    # list of the source files (e.g. .cpp) for the library
)
set(public_headers
    # list of public headers (installed .hpp files) for the library
)
set(public_inlines
    # list of public inline code (installed .inl files) for the library
)
set(headers
    # list of private headers (not installed) for the library
)
set(inlines
    # list of private inline code (not installed) for the library
)
# this is optional
set(resources
    # list of files or directories that contains library specific resources (shaders, images, data, ...)
    Shaders
)

# configuring the target as a dynamic library
add_library(
        ${NAME_OF_LIBRARY} SHARED
        ${sources}
        ${headers}
        ${inlines}
        ${public_headers}
        ${public_inlines}
        ${resources}
)

# Setting specific target_* properties
... 
target_link_libraries(
        ${NAME_OF_LIBRARY}
        PUBLIC
        # Give here the list of the dependency (e.g. Radium::Core ... ) to link with
)

# Configuring the library to be inserted into the Radium exosystem
message("Configure library ${NAME_OF_LIBRARY} for insertion into Radium exosystem")
configure_radium_library(
        TARGET ${NAME_OF_LIBRARY}
        NAMESPACE ${NAME_OF_LIBRARY} # The imported target will be ${NAME_OF_LIBRARY}::${NAME_OF_LIBRARY}
        PACKAGE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake
        PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
        FILES "${public_headers};${public_inlines}" # They will be installed in <prefix>/include/${NAME_OF_LIBRARY}/
)
# Optional, if the library need resources
installTargetResources(
        TARGET ${NAME_OF_LIBRARY}
        DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/Shaders
)
~~~

## Configuring a set of libraries as a single package
When configuring a library, a cmake package configuration file should be written so that the cmake package configuration module is installed alongside the library.
Meanwhile, when several libraries must be used as components in a single package (e.g. the Radium internal libraries are all gathered into the single Radium package), a more general configuration module has to be defined.

For this, instead of defining a configuration package for each configured library, the parameter `PACKAGE_CONFIG` of the function [configure_radium_library](#configure_radium_library) should be omitted
and the function [configure_radium_package](#configurePackage) should be used.

Standard usage of this function requires to have some libraries configured like the following: 
~~~{.cmake}
...
configure_radium_library(
    TARGET <firstLib>
    FILES "${firstLib_public_headers}"
    TARGET_DIR <include_prefix>
    NAMESPACE <namespace>
    PACKAGE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake
)
...
configure_radium_library(
    TARGET <secondLib>
    FILES "${secondLib_public_headers}"
    TARGET_DIR <include_prefix>
    NAMESPACE <namespace>
    PACKAGE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake
)
...
~~~ 

Once this is done, the package should be configured using
~~~{.cmake}
...
configure_radium_package(
    NAME <packageName>
    PACKAGE_DIR ${CMAKE_INSTALL_PREFIX}/lib/cmake
    PACKAGE_CONFIG ${CMAKE_CURRENT_SOURCE_DIR}/<packageName>Config.cmake.in
)
~~~ 

Where the configuration file `<packageName>Config.cmake.in` should be written as the example below. 
(see [cmake documentation](https://cmake.org/cmake/help/v3.13/manual/cmake-packages.7.html) for deeper explanations on how to write a package configuration file).
~~~{.cmake}
# list the supported component components
set(<packageName>_supported_components <firstLib> <secondLib>)

# mark the package as found
set(<packageName>_FOUND True)

# verify the list of requested component. If none is given, request for all components
if (NOT <packageName>_FIND_COMPONENTS)
    set(<packageName>_FIND_COMPONENTS ${<packageName>_supported_components})
endif()

# search for requested components
foreach(_comp ${<packageName>_FIND_COMPONENTS})
  list(FIND <packageName>_supported_components ${_comp} ${_comp}_FOUND)
  if (${${_comp}_FOUND} EQUAL -1)
    set(<packageName>_FOUND False)
    set(<packageName>_NOT_FOUND_MESSAGE "Unsupported <packageName> component: ${_comp}")
  else()
    set(${_comp}_FOUND True)
  endif()
endforeach()

# configure individual components
#------------------------------------------------------------------------------------------------------------
include(CMakeFindDependencyMacro)

# component <firstLib>
if (<firstLib>_FOUND)
    # manage component dependencies (e.g. some Radium components)
    if ( NOT Radium_FOUND)
        find_dependency(Radium COMPONENTS Core Engine REQUIRED)
    endif()
    # include the target definition generated by configure_radium_library (must be installed in the same directory than the package module)
    include("${CMAKE_CURRENT_LIST_DIR}/<firstLib>Targets.cmake" )
endif()

# component <secondLib>
if (<secondLib>_FOUND)
    # manage component dependencies (e.g. <firstLib> must be requested and <secondLib> depends also on Qt5)
    if (NOT <firstLib>_FOUND)
        set(<secondLib>_FOUND False)
        set(<packageName>_FOUND False)
        set(<packageName>_NOT_FOUND_MESSAGE "Component <secondLib> requires the component <firstLib>")
        # Note that you can also explicitely configure first lib instead of raising an error
    else()
        if (NOT Qt5_FOUND)
            find_dependency(Qt5 COMPONENTS Core Widgets REQUIRED)
        endif()
        include("${CMAKE_CURRENT_LIST_DIR}/<secondLib>Targets.cmake" )
    endif()
endif()
~~~ 

Based on the above example, once the package `<packageName>` is found, the targets `<namespace>::<firstLib>` and  `<namespace>::<secondLib>` will be imported in the current project and should be used as any imported Radium targets.

## Configuring a command line application (all systems)

## Configuring a bundled graphical application (MacOsX only)


