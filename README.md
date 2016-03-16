# Radium-Engine
Coolest engine ever made #yolo #swag

## Dependencies 
* Eigen 3.2+ (in repository)
* Assimp (included as a submodule)
* OpenGL 3+
* Qt Core, Qt Widgets and Qt OpenGL v5.4+ 
* GLEW (used on Windows only for now)
* FreeImage (for texture loading)
* To build : CMake 2.8.11+

## Getting submodules (for older versions)
```
$ git submodule init
$ git submodule update
```

## Compile-time configuration
This is the list of options that can be configured when compiling. By default all these options are turned off
but you can enable them by uncommenting the corresponding `#define` or add it to the definitions when running cmake. 
* `CORE_USE_DOUBLE` (in `CoreMacros.hpp`): Switch the default floating point type to `double` instead of `float`.
* `FORCE_RENDERING_ON_MAIN_THREAD` (in `Viewer.cpp`) : disables asynchronous rendering.

## Building on Linux
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```
The compiled application can be found in `bin` or `bin-debug`, depending on
how you built your stuff.

## Qt cmake errors
In case you run into an error like
```
By not providing "FindQt5Widgets.cmake" in CMAKE_MODULE_PATH this project
has asked CMake to find a package configuration file provided by
"Qt5Widgets", but CMake did not find one.
```
you need to set `CMAKE_PREFIX_PATH`, pointing to your Qt root dir.
E.g for the default setup configuration from the web installer on linux you would have
```
$ cmake -DCMAKE_PREFIX_PATH=/opt/Qt/5.x/gcc_64
```
## Building on Windows with Visual Studio.

### Supported versions of MSVC
Since Radium requires the C++11 advanced features such as constexpr, we need a recent MSVC
* *VS 2015 Community* is strongly advised (https://www.visualstudio.com/products/visual-studio-community-vs)
* *VS 2013* with the november 2013 CTP (corrective patch)[https://www.microsoft.com/en-us/download/confirmation.aspx?id=41151} 
 
### Dependencies
* On windows GLEW is required for all the fancy OpenGL functions such as `glBindBuffers` (sad but true...).
 * Dowload GLEW and create a Glew folder in 3rdPartyLibraries (TODO : so that CMake finds it automagically).

### Build

* Use cmake-gui and set the `CMAKE_PREFIX_PATH` in the cache (see above) to the Qt base folder.
* The assimp directory will be compiled automatically (if you have correctly submodule init / update) 
* The GLEW directory should be automatically detected if it is in your 3rd party folder. If not, set it manually in the GUI.

### Run

* Don't forget to copy the DLLs in the executable folder (use `depends.exe` to figure out which).
* Set "radium" as your startup project
* Change the application working directory to `$(ProjectDir)..` (go to the "radium" project properties, *Debugging* menu, *Working Directory*) to get the shaders to load properly.

## Documentation
For documentation about particular stuff in the engine (how to develop a plugin, how renderer works, how to setup a scene file, ...),
please refer to the Docs/ folder.
