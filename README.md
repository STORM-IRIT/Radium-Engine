# Radium-Engine
Coolest engine ever made #yolo #swag


## Dependencies 
* Eigen 3.2+ (in repository)
* OpenGL 3+
* QT Core, Widgets and OpenGL v5.4+ 
* To build : CMake 2.8.11+  

## Building
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
