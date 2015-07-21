# Radium Engine programmer manual.

## Source code organization

* `src` contains the main source code
 * `Core` : the Core module (static library) contains the foundation
 classes such as math classes, containers, adapters to the standard
 library, etc. 
 * `Engine` the Engine module (static library) contains all graphics
 related code and engine subsystems running.
 * `Main Application` has the Qt-based GUI and the program entry point.
* `shaders` contains the OpenGL Shaders used by the renderer.
* `3rdPartyLibs` contains any library dependency. CMake will automatically
look in this folder to find the libraries.

## Engine Object Model

### Entities

*Entities* are the base object manipulated by the engine. You can think
of them as "game objects", something that represent an object in the scene.
Entity data include a world space to object space transform and a list of 
*Components*

### Systems and Components

Each *Component* represent an aspect of an *Entity* to a particular engine subsystem.
For example, an animated character may have a skeleton(animation component),
a high-resolution mesh (display component) and a physics component (a convex shape).

Each Component is related to a *System*. The Engine loads several Systems (statically
or from a plugin) and they keep the Components of all Entities in the scene updated.


