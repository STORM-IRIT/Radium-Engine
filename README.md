# Radium-Engine
Coolest engine ever made #yolo #swag

See [this presentation](https://docs.google.com/presentation/d/12W2KXY7ctJXFIelmgNEn7obiBv_E4bmcMl3mXeJPVgc/edit?usp=sharing)
for an overview of the project.

## Dependencies 
* Eigen 3.2 (in repository)
* Assimp 3.2 (included as a submodule)
* OpenGL 3+ / GLSL 330
* Qt Core, Qt Widgets and Qt OpenGL v5.4+ 
* GLEW (used on Windows only for now)
* stb_image (in repository)
* To build : CMake 2.8.11+

## Supported compiler and platforms

The following platforms and tool chains have been tested and should work :

* *Windows* : MSVC 2015 or higher, MinGW-32 4.9.2 or higher (with Qt Creator).
* *Mac OSX* : clang with XCode
* *Linux* : gcc 4.8 or higher, clang

## Build instructions

### Getting submodules 
Assimp is a submodule : you can get it by running these two commands
```
$ git submodule init
$ git submodule update
```

### Configure build

Radium offers two build options which are off by default :
* `USE_DOUBLE` sets the floating point format to double-precision instead of single precisition
* `USE_OMP` instructs the compiler to use OpenMP

###  Building on Linux

Building on linux should be pretty straightforward, provided that cmake can locate the dependencies.
You will need to have the openGL headers and libraries, Qt 5.4 or more and cmake.

```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

If cmake doesn't locate the Qt files (e.g. if you manually installed Qt as opposed to using your distribution's package),
see the troubleshooting section below.

The compiled application can be found in `bin`. Default plugins DLL are compiled in
`Plugins/bin` by default.

### Building on Windows with Visual Studio

#### Supported versions of MSVC
Since Radium requires the C++11/C++14 advanced features such as `constexpr`, you will need a recent MSVC
* *VS 2015 Community* is strongly advised (https://www.visualstudio.com/products/visual-studio-community-vs)
* *VS 2013* with the november 2013 CTP (corrective patch)  should work, but is untested.
[https://www.microsoft.com/en-us/download/confirmation.aspx?id=41151]
 
#### Dependencies

*Qt* distributes version 5.6 with precompiled libraries for VS 2015 - 64 bits. 
If using earlier versions of Qt (5.4 or 5.5)  or a different toolset you may have to compile Qt yourself.
You will probaby have to manually point cmake to the Qt folder (see Troubleshooting below)

On windows GLEW is required for all the fancy OpenGL functions such as `glBindBuffers` (sad but true...).
Dowload GLEW [http://glew.sourceforge.net/].
You can put the GLEW folder in `3rdPartyLibraries` where cmake will look for it. If GLEW is somewhere else,
you will have to manually set the variables in cmake.

The default cmake configuration for Assimp should be correct.

#### Building

As long as cmake run smoothly the engine should compile.

### Run

* Don't forget to copy the third party DLLs in the executable folder :
 * glew32.dll
 * Qt libraries (Qt5xxx.dll or Qt5xxxd.dll if you are in debug) : Core, Gui and Widgets
* Set "radium" as your startup project
* Change the application working directory to `$(OutDir)..` (go to the "radium" project properties, *Debugging* menu, *Working Directory*) to get the shaders to load properly.

## Building on Windows with QtCreator / MinGW

### Dependencies 

Unfortunately, GLEW does not have MinGW binaries, so you have to get the GLEW source and compile it.
* Download the latest glew soure (http://glew.sourceforge.net/)
* Move the GLEW source folder to  `\Glew`  in `3rdPartyLibraries`
* Build GLEW in "Release" with Qt creator.
* Move `libglew32.a`and `libglew32.dll.a` in `3rdPartyLibraries\Glew\lib\Release\mingw`

### Building

You can now run Cmake in Qt Creator. It should locate Qt libs automatically. 
It should also find GLEW provided you followed the previous step. 
If not, you can always set `GLEW_INCLUDE_DIR` and `GLEW_LIBRARY` manually to their correct locations.

### Running

If you run the program within QtCreator the path should be correctly set, and the executable should run fine.

However, if you run it directly, you will need to put all the dlls within the `bin`folder :
* `libgcc_s_dw2-1.dll`
* `libstdc++-6.dll`
* `libwinpthread-1.dll`
* Qt Core, Gui and Widgets libraries

They are found in the Qt distribution folders : e.g. `\Qt\5.x\mingwXX_32\bin`

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
```
$ cmake -DCMAKE_PREFIX_PATH=/opt/Qt/5.x/gcc_64
```

On windows, using cmake-gui you can use the "add entry" button, adding `CMAKE_PREFIX_PATH` 
as a string to point to the Qt directory (for example in the default installation :
`C:/Qt/5.6/msvc2015_64` )

### Plugins build

Remember that the plugins depend from the engine libs (Core and Engine) but there is no
build dependency between the main application and the plugins. Thus you should be careful
of plugins not being rebuilt when rebuilding just the main application.

### Shaders not found

The shaders are looked in a folder relative to the `bin`folder. Make sure your working directory is the 
folder where the radium app executable lives.

## Documentation
For more documentation about the engine (how to develop a plugin, 
how renderer works, how to setup a scene file, ...), please refer to the Docs/ folder.
