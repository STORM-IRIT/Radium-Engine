\page develmaterials API: Material management
[TOC]

A Ra::Engine::Material is a way to control the appearance of an object when rendering. 
A Ra::Engine::Material defines the _Bidirectional Scattering Distribution function (BSDF)_ to be applied on an object. 

A material is associated to the render geometry of an object (a Ra::Engine::RenderObject) through a
Ra::Engine::RenderTechnique. 

A Ra::Engine::RenderTechnique describes how to use a Ra::Engine::Material to render an object in OpenGL and consists in 
a set of named Ra::Engine::ShaderConfiguration that encompass the OpenGL representation of a Material (glsl code and 
associated data) with, at least, vertex and fragment shaders. The name of each Ra::Engine::ShaderConfiguration 
corresponds to the way a Ra::Engine::Renderer manages its rendering loop.

A Ra::Engine::ShaderConfiguration is associated with a Ra::Engine::ShaderProgram that the Ra::Engine::Renderer will bind
when rendering an object.

The Radium Engine exposes some predefined materials, the _Radium Material Library_, with render techniques corresponding
to the Ra::Engine::ForwardRenderer default renderer. 

## Content of the Radium Material Library
The Radium Material Library defines two default material :
  - BlinnPhong, Ra::Engine::BlinnPhongMaterial, corresponding to the Blinn-Phong BSDF.
  - Plain, Ra::Engine::PlainMaterial, corresponding to a diffuse, lambertian BSDF.

The _Radium Material Library_ can be used as this by any Radium Application or can be extended by an application or a 
Radium Plugin by implementing the corresponding interfaces as described in the 
[Extending the _Radium Material Library_ ](#extend-mtl-lib).

For each material of the library, a default Ra::Engine::RenderTechnique, corresponding to the standard usage of the material by 
the Ra::engine::ForwardRenderer and an optional Ra::Engine::EngineMaterialConverters::ConverterFunction are made 
available through the dedicated factories Ra::Engine::EngineRenderTechniques and  Ra::Engine::EngineMaterialConverters.
See the [Material registration into the Engine](#registration-mtl-lib) section of this documentation to learn more about these factories.


## Simple usage of materials {#simple-mtl-usage}

A simple usage of material is demonstrated in the [HelloRadium Application](https://github.com/STORM-IRIT/Radium-Apps).

When building scene to render, a Ra::Engine::Component must be added to a system as described into the 
_Radium Engine programmer manual_.
A component olds one or several Ra::Engine::RenderObject that will be drawn when rendering.

To define a Ra::Engine::RenderObject and add it to the component, the geometry of a 3D object (a Ra::Engine::Mesh) 
must be associated with a Ra::Engine::RenderTechnique that links to the required Ra::Engine::Material.
 
To do that, the following steps must be done :
1. Create the Ra::Engine::Mesh (see the documentation about Meshes)
\snippet HelloRadium/minimalradium.cpp Creating the Engine Mesh

2. Create the Ra::Engine::Material
\snippet HelloRadium/minimalradium.cpp Creating the Material

3. Create the Ra::Engine::RenderTechnique and associate it to the material
\snippet HelloRadium/minimalradium.cpp Creating the RenderTechnique

4. Create the Ra::Engine::RenderObject and add it to the Ra::Engine::Component
\snippet HelloRadium/minimalradium.cpp Creating the RenderObject

## The material interfaces {#interfaces-mtl-lib}
A material is defined by two programming interfaces. The Ra::Engine::Material that defines the C++ interface made 
available for applications and plugins and one defining a [GLSL interface](#glsl-mtl-lib) that allows shader reuse and 
composition for OpenGL rendering.

### C++ interface
The Ra::Engine::Material interface defines the internal abstract representation of a Material. 
This interface defines all the methods required to parametrized the OpenGL pipeline for rendering and will be used 
mainly by the Ra::Engine::RenderTechnique and the Ra::Engine::Renderer classes.

### GLSL interface
Being able to compose shaders in a specific renderer while taking profit of Radium Material Library
(either included in the base engine or defined in plugins) require a clean definition of appearance computation
process and the definition of a glsl interface.

#### Appearance computation needs
In order to compute the appearance of an object, and according to the OpenGL/GLSL approach of rendering, several
aspects might be taken into account and might be integrated into the interface definition to make a material 
renderer-agnostic. 

Note that not all these aspects are required for all materials and all renderers. 
The user or the implementer could rely on default implementation, provided by the _Radium Material Library_ of most of 
these fonctionalities and concentrate its effort in developping the required functionalities for its material/renderer. 

1. Accessing or computing [appearance attributes that depends on geometric data](#vrtx-attr-interface), such as
    - vertex attribs (normal, position, color, ...)
2. Accessing or computing [microgeometric attributes](#microgeometry-interface) that depends on extension of geometric data, such as
    - Normal maps
    - Displacement maps
    - Transparency maps
3. Accessing and [computing appearance attributes that depends on the BSDF model](#bsdf-interface) used to render the object, such as
    - Blinn-Phong BSDF (default BSDF in Radium Engine)
    - Microfacet - based BSDF (added by plugins such as GLTF-2 or PBRT)

#### Vertex attrib interface {#vrtx-attr-interface}
In order to compute the appearance of an object, one need to rely on parameters defined directly on the geometry of
the object. Such parameters (position, normal, tangent, ...) are passed to the shader systems as vertex attributes.

In order to keep the appearance computation agnostic on the way vertex attribs are named or accessed, we must
propose an abstract interface. But, and this is particular to these attributes, one can access to the attributes
himself, on the vertex, or to the attributes interpolated by the rasterizer, on the fragment.
Accessing the Attribute directly on the vertex (i.e. on a vertex shader) does not necessitate an interface as
each shader must define its attributes.

The Vertex attrib interface, to be used in either a fragment shader or a vertex shader is the following. 
Note that, in order to keep the independance between part of shaders, this interface must be define everywhere, even if 
no vertex attribs are accessed. In this case, the default code must be used.

Note also that the attributes accessed through the Vertex attrib interface **must** be defined in world space.
Even if not necessarilly efficient (some transformations might be computed twice), this will ensure more simple
lighting computation. This might be changed in future version of Radium.

In the file that need the **declaration** of the interface, the following lines must be written. 
Note that not all the functions must be pre-declared and the programmer could restrict himself to the only functions 
he needs :

~~~{.cpp}
// Return the Vertex position, in world space, defined or interpolated from vertices
vec4 getWorldSpacePosition();

// Return the geometric normal, in world space, defined or interpolated from vertices
vec3 getWorldSpaceNormal();

//Return the geometric tangent, in world space, defined or interpolated from vertices
vec3 getWorldSpaceTangent();

//Return the geometric bi-tangent, in world space, defined or interpolated from vertices
vec3 getWorldSpaceBiTangent();

//Return the 2D parametric position of the vertex (the texture coords), defined or interpolated from vertices
vec3 getPerVertexTexCoord();

// Return the base color, defined or interpolated from vertices
vec4 getPerVertexBaseColor();

// return the specular color, defined or interpolated from vertices
vec3 getPerVertexSpecularColor();
~~~
Note also that if a function is not needed by a shader, there is no need to implement its interface.

The default implementation of the interface, for accessing interpolated attributes, is the following and could be 
included in a glsl fragment shader by `#include "VertexAttribInterface.frag.glsl"`. 
In order to use this interface, the user must ensure that the attributes are computed in the vertex shader in 
**world space**:
~~~{.cpp}
//----------------- Supported vertex Attribs ---------------------
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_texcoord;
layout (location = 2) in vec3 in_normal;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_viewVector;
layout (location = 5) in vec3 in_lightVector;
layout (location = 6) in vec3 in_color;

//------------------- VertexAttrib interface ---------------------
vec4 getWorldSpacePosition() {
    return vec4(in_position, 1.0);
}

vec3 getWorldSpaceNormal() {
    if (length(in_normal.xyz) < 0.0001) { // Spec GLSL : vector not set -> (0, 0, 0, 1)
        vec3 X = dFdx(in_position);
        vec3 Y = dFdy(in_position);
        return normalize(cross(X, Y));
    } else {
        return normalize(in_normal);
    }
}

vec3 getWorldSpaceTangent() {
    if (length(in_tangent.xyz) < 0.0001) { // Spec GLSL : vector not set -> (0, 0, 0, 1)
        return normalize(dFdx(in_position));
    } else {
        return normalize(in_tangent);
    }
}

vec3 getWorldSpaceBiTangent() {
    return normalize(cross(in_normal, in_tangent));
}

vec3 getPerVertexTexCoord() {
    return in_texcoord;
}

vec4 getPerVertexBaseColor() {
    return vec4(in_color, 1.0);
}
~~~

#### Microgeometry interface {#microgeometry-interface}
Defining the micro-geometry procedurally or by using textures allows to decorelates the geometric sampling from the
appearance parameters sampling.
The best example of procedural micro-geometry is normal mapping.

For a practical introduction to this kind of approach with opengl, the reader could refer to the
[Learn opengl tutorial](https://learnopengl.com/).

For a more indepth presentation of these kind of techniques for realtime rendering, we encourage the reader to refer to
[Real-Time Rendering, Fourth Edition, by Tomas Akenine-Möller, Eric Haines, Naty Hoffman, Angelo Pesce, Michał Iwanicki, and Sébastien Hillaire](https://www.realtimerendering.com/).

The microgeometry could also define which fragment is transparent.
So, in order to be able to compute or discard transparent fragments, one need to define a `toDiscard` function.

The interface (to be implemented in the file `name_of_the_BSDF.glsl`) is then

~~~{.cpp}
// Return the world-space normal computed according to the microgeometry definition`
// If no normal map is defined, return N
vec3 getNormal(Material material, vec2 texCoord, vec3 N, vec3 T, vec3 B);

// return true if the fragment must be condidered as transparent (either fully or partially)
bool toDiscard(Material material, vec4 color);
~~~


#### BSDF interface {#bsdf-interface}
Implementing or using the GLSL BSDF interface is based on the fact that the method Ra::Engine::Material::getMaterialName()
 must return a string that contains the `name_of_the_BSDF` implemented in a file named `name_of_the_BSDF.glsl`.
This file is preloaded at [material registration](#registration-mtl-lib) into a `glNamedString` to allow inclusion by others.

In order to be composable by Radium applications and renderers, this glsl file must only contains the implementation
of the BSDF interface, with no `void main(){...}` nor access to vertex attribs or so on ...

This file must contain an inclusion guard :
~~~{.cpp}
#ifndef METALLICROUGHNESS_GLSL
#define METALLICROUGHNESS_GLSL
.
.
.
#endif
~~~

This file must define the following

~~~{.cpp}
// Concrete definition of the Material structure that contains the BSDF parameters
struct Material {
    ...
};

// Returns the base color, or albedo, of the material, at the surface coordinates defined by texCoord.
// The returned color will eventually be used to infer if the fragment is transparent or not.
// The alpha channel could then vary from 0 (totally transparent) to 1 (totally opaque)
vec4 getBaseColor(Material material, vec2 texCoord);

// Returns the so called "Diffuse Color" of the material, at the surface coordinates defined by texCoord.
// This could be the same that the base color or obtained by a more or less complex computation (Fresnel, ...)
// The alpha channel is the same than the one computed for getBaseColor().
vec4 getDiffuseColor(Material material, vec2 texCoord);

// Returns the so called "Specular Color" of the material, at the surface coordinates defined by texCoord.
// This could be the same that the base color (without the alpha channel) or obtained by
// a more or less complex computation
vec3 getSpecularColor(Material material, vec2 texCoord);

// Return the bsdf value for the material, at surface coordinates defined by texCoord,
// for the incoming and outgoing directions `wi` and `wo`. These directions MUST be in local frame.
// The local Frame is the Frame wher the Geometric normal is the Z axis,
// and the tangent defined the X axis.
vec3 evaluateBSDF(Material material, vec2 texCoord, vec3 wi, vec3 wo);
~~~


## Extending the Radium Material Library {#extend-mtl-lib}
The Radium Engine API defines several interfaces and factories for material management that allows easy extensibility 
of the Radium Material Library.

### Material registration into the Engine {#registration-mtl-lib}

### Implementing the Material interface {#mtlimpl-mtl-lib}

When implementing this interface, and in order to make the material available to all applications or Plugins,
two static methods to register and unregister the material into the _Material_ .
These method could have the following profiles :
~~~{.cpp}
class MyMaterial : public Ra::Engine::Material {
public:
    // implementation of the abstract interface
    ...
private:
    // data member for MyMaterial
    ...
public:
    static void registerMaterial();
    static void unregisterMaterial();
    // MyMaterial specific public interface
    ...
}
~~~
