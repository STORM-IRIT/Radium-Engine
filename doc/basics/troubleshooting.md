\page basicsTroubleshooting Troubleshooting
[TOC]

## Qt cmake errors
In case you run into an error like
~~~
By not providing "FindQt5Widgets.cmake" in CMAKE_MODULE_PATH this project
has asked CMake to find a package configuration file provided by
"Qt5Widgets", but CMake did not find one.
~~~
you need to set `CMAKE_PREFIX_PATH`, pointing to the Qt root dir of your commpiler.
For example on linux with gcc :
~~~bash
$ cmake -DCMAKE_PREFIX_PATH=/opt/Qt/5.x/gcc_64
~~~

On windows, using cmake-gui you can use the "add entry" button, adding `CMAKE_PREFIX_PATH`
as a string to point to the Qt directory (for example in the default installation :
`C:/Qt/5.6/msvc2015_64` )

## Crash when starting main application on windows
This is usally caused by missing dlls.
With Visual Studio, you may need to copy the Qt dlls to Radium bin folder `Bundle-MSVC\bin` or `Bundle-MSVC-Debug\bin`.
