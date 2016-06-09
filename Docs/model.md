# Radium Engine programmer manual

## Source code organization
* `src` contains the main source code
 * `Core` : the Core module (static library) contains the foundation
 classes such as math classes, containers, adapters to the standard
 library, etc. 
 * `Engine` the Engine module (static library) contains all graphics
 related code and engine subsystems running.
 * `Main Application` has the Qt-based GUI and the program entry point.
 * `Plugins` contains the plugins which add Systems and Components to the engine.
* `Shaders` contains the OpenGL Shaders used by the renderer.
* `3rdPartyLibs` contains any library dependency. CMake will automatically
look in this folder to find the libraries if they are not installed on your system.

### Import and export
Core and Engine (and the plugins) are compiled as dynamic libraries, therefore you must be careful about which symbols need exporting, especially on Windows. Every module has a specific header file (`RaCore.hpp` for Core) which must be included first in all other headers of the module. In particular it defines the `*_API` macros which help exporting and importing symbols on Windows.

## Core Libs
`Core` contains most of the basic code on which the rest of the software is built.
* `CoreMacros.hpp` has definitions of basic types, build configuration and useful macros.
* `Math` is our math library, which is a wrapper around *Eigen*. The most useful files are `Math.hpp` which has mathematical constants and simple functions, and `LinearAlgebra.hpp` which contains the type definitions of most basic vector and matrix types.
* `Containers` has some specially useful std-like containers, most importantly `VectorArray.hpp` which defines a dynamic aray of vectors with both a`std::vector`-like interface and compatibility with Eigen. 
* `Mesh` contains our basic mesh geometry primitives, including the representation of a simple triangle mesh and many functions to operate on geometry.
* `String` contains utilities extending `std::string`. 
* `Log` is a wrapper around the header-only *EasyLogger* library which allows us to log various events.
* `Time` contains utilities around `std::chrono` for precise timings.
* `Tasks` contains the definition for the basic Tasks system and the task queue.
* `Utils` contains generic utilities such as a Singleton template.

## Engine Object Model

### Entities

*Entities* are the base object manipulated by the engine. You can think
of them as "game objects", something that represent an object in the scene.
Entity data include a world space to object space transform and a list of 
*Components*

The entity's role is only to hold together this transform and the list of components.
The entity uses double buffering to prevent transforms from being updated
more than once per frame.

When creating an entity, if you set its transform, *do not forget* to call
`Entity::swapTransformBuffers`, this might prevent you some headache. Example :
```
Ra::Engine::Entity* entity = theEntityManager->getOrCreate( "MyEntity" );
Ra::Core::Transform transform( Ra::Core::Transform::Identity() );
transform.translation = Ra::Core::Vector3( 42, 13, 37 );
entity->setTransform( transform );
entity->swapTransformBuffers(); 
```

### Systems and Components

Each *Component* represent an aspect of an *Entity* to a particular engine subsystem.
For example, an animated character may have a skeleton(animation component),
a high-resolution mesh (display component) and simplified convex shape for collision detection (physics component).

Each Component is related to a *System*. The Engine loads several Systems (statically
or from a plugin) and they keep the Components of all Entities in the scene updated.

For example, an Entity may have a Physics Component (which represent its collision shape), 
and the engine's corresponding Physics System will update its position according to the physics
engine's result.

At each frame, each System is given the opportunity to add *Tasks* to a task queue. Tasks are processed in parallel
during every frame. Tasks can have dependencies between them (e.g. the physics update can wait for the animation update to complete).

### Rendering

Each Component may have some *RenderObjects* (aka Drawables) which are (usually) OpenGL objects. When a Component has 
changed it needs to tell its drawables to update their internal data (such as OpenGL VBOs).

The Render Objects are drawn by the *Renderer* which may live in a separated thread. Each frame it grabs all 
the drawables (which should be double-buffered in case we are mid-VBO update) and calls `draw()` on them.

(TODO : a nice class schema).

### Object manipulation through Editables.

Components and Entity implement the `EditableInterface` which allow them to be manipulated through the UI. Editable objects offer a list of *Properties* on demand. Each property must be unique (have a unique name) within the object instance.

A Property itself is a structure packing primitive objects of different types (scalars, vectors, quaternions, etc.) impemented through a variant-like structure (` EditablePrimitive`). Each primitive can be marked as read-only within the property, and any editor should prevent changing this primitive within the property (this can be enforced by your editable but it's not done automatically).

The archetypal example of a property is a Transform, with its primitives being for example a position, rotation and scale. By default, all entities have only one editable property which is their transform, while Components have no editable properties.

### The system Entity and Debug Display.

One Entity is always created when the engine starts, called the System Entity. Implemented as a singleton, this entity acts as a special object that the engine can use internally to display elements not related to any other Entity. The System Entity is fixed to the world and cannot be moved.

In particular it is expected that UI elements (e.g.  Gizmos) should be attached to the UI component. The Debug Component is useful to display debug primitives which can be useful for graphic debugging.
A series of macros `RA_DISPLAY_...` are defined to conveniently add basic display objects such as points, vectors, lines or 3D frames from anywhere in the code. A compile-time switch (`RA_DISABLE_DEBUG_DISPLAY`) can be activated to disable this feature.

## The plugin framework

Entities and components have been designed so that the engine is modular in terms of features. It is expected that most interesting works will be done by Systems defined in *Plugins*. Each plugin can define its System (and the corresponding Components). 

We use a compile-time plugins loading mechanism. When running `cmake`, it will list the contents of the  `src/Plugins/` directory and add them to be compiled with the projet, and automatically generate the code to include the plugins Systems in the main application.

### Requirements

For this automated build to work the plugins are required to follow these requirements
* The plugin code must be in a subfolder of the `src/Plugins/` directory.
* The folder name is taken to be the _base name_ of the plugin. If the directory name is `BaseName`, then
 * Plugin code must be in `namespace BaseNamePlugin`
 * The system defined by the plugin must be named `BaseNamePlugin::BaseNameSystem`
 * It must be defined in a header located at the top of the plugin subfolder, named `BaseNameSystem.hpp` (its full path should be `src/Plugins/Basename/BasenameSystem.hpp`)
 * The system must inherit from `Engine::System` and have a default empty constructor.


### Tasks and dependencies

Tasks are created by Systems at the beginning of a frame and run concurrently.
Each frame starts by querying all existing systems and collects all the tasks.
During the frame tasks are attributed to threads by the task queue.

### Default plugins

See the structure of the default plugins (e.g. `FancyMesh`) for an example of a working plugin.
