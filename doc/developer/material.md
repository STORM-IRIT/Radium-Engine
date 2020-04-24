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

A Ra::Engine::ShaderConfiguration is associated with a Ra::Engine::ShaderProgram bound by the Ra::Engine::Renderer 
when rendering an object.

The Radium Engine exposes some predefined materials, the _Radium Material Library_, with render techniques corresponding
to the Ra::Engine::ForwardRenderer default renderer. 

# Content of the Radium Material Library
The Radium Material Library defines two default material :
  - BlinnPhong, Ra::Engine::BlinnPhongMaterial, corresponding to the Blinn-Phong BSDF.
  - Plain, Ra::Engine::PlainMaterial, corresponding to a diffuse, lambertian BSDF.

The _Radium Material Library_ can be used as this by any Radium Application or can be extended by an application or a 
Radium Plugin by implementing the corresponding interfaces as described in the 
[Extending the _Radium Material Library](#extend-mtl-lib).

For each material of the library, a default Ra::Engine::RenderTechnique, corresponding to the standard usage of the 
material by the Ra::engine::ForwardRenderer and an optional Ra::Engine::EngineMaterialConverters::ConverterFunction, 
used when loading files to convert file representation of a Material to Radium representation of this material, are 
made available through the dedicated factories Ra::Engine::EngineRenderTechniques and 
Ra::Engine::EngineMaterialConverters.

See the [Material registration into the Engine](#registration-mtl-lib) section of this documentation to learn more about
these factories.

# Simple usage of materials {#simple-mtl-usage}

A simple usage of material is demonstrated in the [HelloRadium Application](https://github.com/STORM-IRIT/Radium-Apps).

When building scene to render, a Ra::Engine::Component must be added to a system as described into the 
_Radium Engine programmer manual_.
A component holds one or several Ra::Engine::RenderObject that will be drawn when rendering.

To define a Ra::Engine::RenderObject and add it to the component, the geometry of a 3D object (a Ra::Engine::Mesh) 
must be associated with a Ra::Engine::RenderTechnique that links to the required Ra::Engine::Material.
 
To do that, the following steps must be done :
1. Create the Ra::Engine::Mesh (see the [documentation about Meshes](@ref develmeshes))
\snippet HelloRadium/minimalradium.cpp Creating the Engine Mesh

2. Create the Ra::Engine::Material
\snippet HelloRadium/minimalradium.cpp Creating the Material

3. Create the Ra::Engine::RenderTechnique, here using the Ra::Engine::EngineRenderTechniques factory, and associate it 
to the material
\snippet HelloRadium/minimalradium.cpp Creating the RenderTechnique

4. Create the Ra::Engine::RenderObject and add it to the Ra::Engine::Component
\snippet HelloRadium/minimalradium.cpp Creating the RenderObject

Note that this way of using the _Radium Material Library_ is very related to the default Radium rendering capabilities
exposed by the [Radium forward renderer](@ref forwardrendererconcept). 
See the [Render technique management](./rendertechnique) documentation to learn how to create your own 
Ra::Engine::RenderTechnique, potentially without associated material.

If one wants to render objects without BSDF computation but with a specific color computation for the fragment, 
follow the guidelines from [the dedicated section](#non-bsdf-rendering) of this documentation.

# Extending the Radium Material Library {#extend-mtl-lib}

The _Radium Material Library_ could be extended to handle several _Bidirectional Scattering Distribution function_. 

In order to make these extensions available to each Radium developer, the _Radium Material Library_ defines several 
interfaces and factories for material management. 

A material is defined by two programming interfaces. The Ra::Engine::Material that defines the 
[C++ interface](#cpp-mtl-lib) made available for applications and plugins and a [GLSL interface](#glsl-mtl-lib) that 
allows shader reuse and composition for OpenGL rendering.

The C++ interface is implemented in a `NameOfTheMaterial.hpp/.cpp` source file.

The GLSL interface is composed of several parts :
1. The implementation of a [BSDF interface](#bsdf-interface) and a [micro-geometry interface](#microgeometry-interface) 
in a `NameOfBSDF.glsl` file that will be included in every fragment shaders that need the implementation of the bsdf.
2. The implementation of one or several vertex shaders that will compute the data used by the 
[vertex attrib interface](#vrtx-attr-interface). These shaders are renderer-specific and the programmer must at least 
give one for the Radium default renderer.
3. The implementation of one or several fragment shaders that, by using the 
[vertex attrib interface](#vrtx-attr-interface), will compute the final color of a fragment using the 
[BSDF interface](#bsdf-interface) and the [micro-geometry interface](#microgeometry-interface). These shaders are 
renderer-specific and the programmer must at least give one for the Radium default renderer.

## C++ interface {#cpp-mtl-lib}
The Ra::Engine::Material interface defines the internal abstract representation of a Material. 
This interface defines all the methods required to parametrized the OpenGL pipeline for rendering and will be used 
mainly by the Ra::Engine::RenderTechnique and the Ra::Engine::Renderer classes.

When implementing this interface, and in order to make the material available to all applications or plugins,
two static methods to register and unregister the material into the _Radium Material Library_ must also be developed.
These method will populate the _Radium Material Library_ factories with specific helper functions to use the material
in the default Radium forward renderer.
Mainly, the `registerMaterial()` method will record an helper function to build a material-related 
Ra::Engine::RenderTechnique dedicated to the Radium forward renderer.
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
See the [Render technique management](./rendertechnique) for documentation on how to build such an helper function.

## GLSL interface {#glsl-mtl-lib}
Being able to compose shaders in a specific renderer while taking profit of Radium Material Library
(either included in the base engine or defined in plugins) require a clean definition of appearance computation
process and the definition of a glsl interface.


## Appearance computation needs
In order to compute the appearance of an object, and according to the OpenGL/GLSL approach of rendering, several
aspects might be taken into account and might be integrated into the interface definition to make a material 
renderer-agnostic. 

The user or the implementer could rely on default implementation, provided by the _Radium Material Library_ of most of 
these functionalities and concentrate its effort in developing the required functionalities for its material/renderer. 

1. Accessing or computing [appearance attributes that depends on geometric data](#vrtx-attr-interface), such as
    - vertex attribs (normal, position, color, ...)
2. Accessing or computing [microgeometric attributes](#microgeometry-interface) that depends on extension of geometric data, such as
    - Normal maps
    - Displacement maps
    - Transparency maps
3. Accessing and [computing appearance attributes that depends on the BSDF model](#bsdf-interface) used to render the object, such as
    - Blinn-Phong BSDF (default BSDF in Radium Engine)
    - Microfacet - based BSDF (added by plugins such as GLTF-2 or PBRT)

### Vertex attrib interface {#vrtx-attr-interface}
In order to compute the appearance of an object, one need to rely on parameters defined directly on the geometry of
the object. Such parameters (position, normal, tangent, ...) are passed to the shader systems as vertex attributes.

In order to keep the appearance computation agnostic on the way vertex attribs are named or accessed, we must
propose an abstract interface. But, and this is particular to these attributes, one can access to the attributes
himself, on the vertex, or to the attributes interpolated by the rasterizer, on the fragment.
Accessing the Attribute directly on the vertex (i.e. on a vertex shader) does not necessitate an interface as
each shader must define its attributes and as the [Mesh API](./mesh.md) allows to communicate between C++ and GLSL. 

Note that the attributes accessed through the Vertex attrib interface **must** be defined in world space.
Even if not necessarily efficient (some transformations might be computed twice), this will ensure more simple
lighting computation. 
This might be changed in future version of Radium.

Each glsl component that needs to access to fragment-interpolated attributes must do that through this interface.
The default declaration and implementation of this fragment interface is given in the file 
`Shaders/Materials/VertexAttribInterface.frag.glsl`. This interface might be included in any **fragment shader** that 
want to use it by `#include "VertexAttribInterface.frag.glsl"`.

It relies on some standard vertex attribute that have to be set by the vertex shader with respect to the following 
out binding :
~~~{.cpp}~
// All attributes are given in world space
layout (location = 0) out vec3 position;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 texcoord;
layout (location = 3) out vec3 vertexcolor;
layout (location = 4) out vec3 tangent;
~~~
The default implementation of the fragment interface is robust to inactive attributes, i.e. attributes that are not set 
by the vertex shader.

If one wants to implement its own vertex attribute interface, in order, e.g., to take advantage of some application 
specific data layout, the following declaration of the glsl functions exported by the interface must be done. 
The implementation of the interface could then be developed in the appropriate glsl file.
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


#### Microgeometry interface {#microgeometry-interface}
Defining the micro-geometry procedurally or by using textures allows to de-correlates the geometric sampling from the
appearance parameters sampling.
The best example of procedural micro-geometry is normal mapping.

For a practical introduction to this kind of approach with opengl, the reader could refer to the
[Learn opengl tutorial](https://learnopengl.com/).

For a more indepth presentation of these kind of techniques for realtime rendering, we encourage the reader to refer to
[Real-Time Rendering, Fourth Edition, by Tomas Akenine-Möller, Eric Haines, Naty Hoffman, Angelo Pesce, Michał Iwanicki
 and Sébastien Hillaire](https://www.realtimerendering.com/).

The microgeometry could also define which fragment is transparent.
So, in order to be able to compute or discard transparent fragments, one need to define a `toDiscard` function.

The interface (to be implemented in the file `name_of_the_BSDF.glsl`) is then

~~~{.cpp}
// Return the world-space normal computed according to the microgeometry definition`
// If no normal map is defined, return N
vec3 getNormal(Material material, vec3 texCoord, vec3 N, vec3 T, vec3 B);
// return true if the fragment must be condidered as transparent (either fully or partially)

bool toDiscard(Material material, vec4 color);
~~~


#### BSDF interface {#bsdf-interface}
Implementing or using the GLSL BSDF interface is based on the fact that the method Ra::Engine::Material::getMaterialName()
 must return a string that contains the `name_of_the_BSDF` implemented in a file named `name_of_the_BSDF.glsl`.
This file is preloaded at [material registration](#registration-mtl-lib) into a `glNamedString` to allow inclusion by others.

In order to be composable by Radium applications and renderers, this glsl file must only contains the implementation
of the BSDF interface, with no `void main(){...}`, the implementation of the micro-geometry interface and must only 
access to vertex attribs by using the vertex attribute interface or so on ...

This file must contain an inclusion guard :
~~~{.cpp}
#ifndef METALLICROUGHNESS_GLSL
#define METALLICROUGHNESS_GLSL
.
.
.
#endif
~~~

The BSDF interface consists in the following

~~~{.cpp}
// Concrete definition of the Material structure that contains the BSDF parameters
struct Material {
    ...
};

// Returns the base color, or albedo, of the material, at the surface coordinates defined by texCoord.
// The returned color will eventually be used to infer if the fragment is transparent or not.
// The alpha channel could then vary from 0 (totally transparent) to 1 (totally opaque)
vec4 getBaseColor(Material material, vec3 texCoord);

// Returns the so called "Diffuse Color" of the material, at the surface coordinates defined by texCoord.
// This could be the same that the base color or obtained by a more or less complex computation (Fresnel, ...)
// The alpha channel is the same than the one computed for getBaseColor().
vec4 getDiffuseColor(Material material, vec3 texCoord);

// Returns the so called "Specular Color" of the material, at the surface coordinates defined by texCoord.
// This could be the same that the base color (without the alpha channel) or obtained by
// a more or less complex computation
vec3 getSpecularColor(Material material, vec3 texCoord);

// Return the bsdf value for the material, at surface coordinates defined by texCoord,
// for the incoming and outgoing directions `wi` and `wo`. These directions MUST be in local frame.
// The local Frame is the Frame wher the Geometric normal is the Z axis,
// and the tangent defined the X axis.
vec3 evaluateBSDF(Material material, vec3 texCoord, vec3 wi, vec3 wo);
~~~

## Material registration into the Engine {#registration-mtl-lib}
When implementing the [GLSL interface](#glsl-mtl-lib) of a Material, the user can rely on several 
glsl components defined by the Engine. Glsl components are helper functions and data structure that could
be used to develop specific shaders. 

To make this common component available to users, and also each user defined component that want to be available to 
others, the Radium Engine defines a material component registration system that allows to populate and extend the 
_Radium Material Library_.

The registration and glsl component access system is made of 3 parts
1. Registration of OpenGL/GLSL named string : defining a re-usable GLSL component.
2. Registration of OpenGL/GLSL Program configuration : defining how to link shaders into a program for a specific 
rendering step.
3. Render Technique factory : defining which Program configurations must be used for each renderer-specific task.

a fourth part, optional, could be defined to convert a Ra::Core::Asset:MaterialData (file representation of a material)
to a Ra::Engine::Material.

### Registration of OpenGL/GLSL named string
Relying on already developed GLSL component require that this component could be included in the client code.
In GLSL, this is done using the preprocessor directive ``#include </ComponentPath/ComponentName.glsl>``.
As specified by the ARB_shading_language_include specification, included files must be preloaded in a 
GlNamedString object. The Ra::Engine::ShaderManager provide a way to populate the GLNamedString ecosystem.

To register a GLSL component, from a file named ``SharedComponent.glsl``, so that it could be included in others GLSL files
by ``#include </SharedComponent.glsl>``, one just need to do the following

~~~{.cpp}
    // adding the material glsl implementation file
    ShaderProgramManager::getInstance()->addNamedString(
        "/SharedComponent.glsl", "/absolute/path/to/file/SharedComponent.glsl" );
~~~

### Registration of OpenGL/GLSL Program configuration
The use of GLSL component to render object requires the building of a render-task specific OpenGL Program that link 
together the GLSL component and shaders addressing stages of the OpenGL Pipeline. According to the OpenGL specification, 
A vertex shader and a fragment shader stage are mandatory whereas tesselation end geometry shader are optional and 
depends only of the way one want to configure its pipeline for rendering.
 
To describe the OpenGL program configuration, and make it reusable, the configuration must be registered in the 
Ra::Engine::ShaderConfigurationFactory. To do that, for each reusable configuration, one just need to do the following
~~~{.cpp}
    // build the configuration
    Ra::Engine::ShaderConfiguration myConfig(
        "ConfigName",
        "absolute/path/to/vertexshader.vert.glsl",
        "absolute/path/to/fragmentshader.vert.glsl", );
    // add optional components of the configuration (#define, geometry shaders, ...
    ...
    // Register the configuration to the factory
    Ra::Engine::ShaderConfigurationFactory::addConfiguration( myConfig );
~~~

once registered, a shader configuration could be fetched from the factory by its name :
~~~{.cpp}
auto theConfig =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "ConfigName" );
~~~

### Registering a RenderTechnique
A Ra::Engine::RenderTechnique describes which Ra::Engine::ShaderConfiguration a renderer will use for each of its 
rendering passes. Such a render technique could encompass a Ra::Engine::Material but its meaning is larger than just 
computing the BSDF. 
In order to make a GLSL component that compute the appearance of a 3D object usable by a the default Radium renderer,
one must define which shader configuration to use for each pass of the renderer.  
A Render technique will be used to configure the rendering of a geometry and the association between the geometry and 
the render technique is made in a Ra::Engine::RenderObject.
Making a GLSL component available for the Ra::Engine::ForwardRenderer default renderer in Radium, on must define which
Ra::Engine::ShaderConfiguration to use for the passes Ra::Engine::DefaultRenderingPasses::LIGHTING_OPAQUE, 
Ra::Engine::DefaultRenderingPasses::Z_PREPASS and, if the appearance might be transparent, 
Ra::Engine::DefaultRenderingPasses::LIGHTING_TRANSPARENT.

To do that, the Default render technique, the one that wil be used by Ra::Engine::ForwardRenderer must be registered
into the Ra::Engine::EngineRenderTechniques factory. This is done according to the following

~~~{.cpp}
Ra::Engine::EngineRenderTechniques::registerDefaultTechnique(
        // This name will be used to query the RenderTechnique when rendering
        "NameOfTheTechnique", 
        // This lambda will be executed to configure the rendering technique for a RenderObject
        []( Ra::Engine::RenderTechnique& rt, bool isTransparent ) {
            // Configure the technique to render this object at the mandatory pass
            auto lightpass =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "MyTechniqueOpaque" );
            rt.setConfiguration( lightpass, DefaultRenderingPasses::LIGHTING_OPAQUE );

            // Z prepass (Recommended) : 
            auto zprepass =
                Ra::Engine::ShaderConfigurationFactory::getConfiguration( "MyTechniqueZprepass" );
            rt.setConfiguration( zprepass, DefaultRenderingPasses::Z_PREPASS );
            // Transparent pass (0ptional) : If Transparent ... add LitOIT
            if ( isTransparent )
            {
                auto transparentpass =
                    Ra::Engine::ShaderConfigurationFactory::getConfiguration( "MyTechiqueTransparent" );
                rt.setConfiguration( transparentpass,
                                     DefaultRenderingPasses::LIGHTING_TRANSPARENT );
            }
        } );
~~~

once registered, the render technique could then be associated with any render object using the following principle :
~~~{.cpp}
// Construct and initialize a Ra::Engine::RenderTechnique object
Ra::Engine::RenderTechnique rt;
// Associate a Ra::engine::Material with the render technique if needed
bool isMaterialTransparent = false;
if ( haveMaterial ) {
    std::shared_ptr<Ra::Engine::Material> radiumMaterial( new MyMaterial(...) );
    isMaterialTransparent = radiumMaterial->isTransparent();
    rt.setMaterial( radiumMaterial );
} else {
    rt.setMaterial( nullptr );
}
// configure the render technique for rendering this material with the default renderer
auto builder = Ra::Engine::EngineRenderTechniques::getDefaultTechnique( "NameOfTheTechnique" );
builder.second( rt, isMaterialTransparent );
~~~


## Rendering without using Materials {#non-bsdf-rendering}
The _Radium Material Library_ and related components are mainly designed to manage Materials as a representation of a 
_Bidirectional Scattering Distribution function (BSDF)_.

When rendering, it is sometime useful to compute the final color of an object that do not rely on a bsdf but just 
on a specific color for each geometry fragment.

Even if the Ra::Engine::PlainMaterial could be used for this and assuming that such a way to define material is object 
specific, the following steps are required :

1. Develop specific vertex and fragment shaders to compute the fragment color
2. Build a Ra::Engine::ShaderConfiguration that uses these shaders
3. Build a render technique that use this configuration
4. Associate the render technique with a geometry in a Ra::Engine::RenderObject

This could result in the following C++ code to configure a RenderObject.
~~~{.cpp}
// 1. Develop specific vertex and fragment shaders to compute the fragment color
// Vertex shader source code
const std::string vertexShaderSource{
    "#include \"TransformStructs.glsl\"\n"
    "layout (location = 0) in vec3 in_position;\n"
    "uniform Transform transform;\n"
    "void main(void)\n"
    "{\n"
    "    mat4 mvp = transform.proj * transform.view;\n"
    "    gl_Position = mvp*vec4(in_position.xyz, 1.0);\n"
    "}\n"};
// Fragment shader source code
const std::string fragmentShaderSource{
    "layout (location = 0) out vec4 out_color;\n"
    "vec4 some_color_computation()\n"
    "{\n"
    "    vec result = vec4(vec3(1), 1);\n"
    "}\n"};
    "void main(void)\n"
    "{\n"
    "    out_color = some_color_computation();\n"
    "}\n"};
// 2. Build a Ra::Engine::ShaderConfiguration that uses these shaders
Ra::Engine::ShaderConfiguration myConfig{"MyColorComputation"};
config.addShaderSource( Ra::Engine::ShaderType::ShaderType_VERTEX, vertexShaderSource );
config.addShaderSource( Ra::Engine::ShaderType::ShaderType_FRAGMENT, fragmentShaderSource );
Ra::Engine::ShaderConfigurationFactory::addConfiguration( myConfig );
// 3. Build a render technique that use this configuration
Ra::Engine::RenderTechnique theRenderTechnique;
theRenderTechnique.setConfiguration( myConfig, DefaultRenderingPasses::LIGHTING_OPAQUE );
// 4. Associate the render technique with a geometry in a Ra::Engine::RenderObject
std::shared_ptr<Ra::Engine::Mesh> theMesh( new Ra::Engine::Mesh( "theMesh" ) );
theMesh->loadGeometry( Ra::Core::Geometry::makeSharpBox( {0.1f, 0.1f, 0.1f} ) );
auto theRenderObject = Ra::Engine::RenderObject::createRenderObject(
    "myRenderObject", theRadiumComponent, 
    Ra::Engine::RenderObjectType::Geometry, theMesh, 
    theRenderTechnique );
addRenderObject( theRenderObject );
~~~

After this, when ``theRenderObject`` will be drawn by the Ra::Engine::ForwardRenderer, all the resulting fragments 
will have the color computed by the GLSL function ``some_color_computation()``
