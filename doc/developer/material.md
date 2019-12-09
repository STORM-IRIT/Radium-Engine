\page develmaterials API: Material management
[TOC]

A Material is a way to control the appearance of an object when rendering. It could be the definition of a classical
rendering materials, a _Bidirectional Scattering Distribution function (BSDF)_, or just define the way a geometry
could be rendered and how is computed the final color of an object.

A material is associated to the render geometry of an object (a Ra::Engine::Component in the Radium nomenclature) through a so
called Ra::Engine::RenderTechnique. This association is managed by the Ra::Engine::RenderObject class.

This documentation aims at describing the way materials are managed in the Radium engine and how one can extend the set
of available material, or specialized a renderer with ad-hoc (pseudo)-material.

## Material subsystem and Material Library
The Radium Engine defines an abstract interface for material definition that allows easy extensibility of the Engine.
This section describe the workflow of Material management from asset loading to rendering. Note that, for a given
application or renderer, a Material could be defined directly without loading it from a file.

### The MaterialData interface
The interface Ra::Core::Asset::MaterialData define the external representation of a material. Even if this interface could be
instantiated, it defines an abstract material that is not valid for the Engine.
This interface must then be implemented to define materials that could be loaded from a file.

When defining a loadable material, the corresponding implementation must set the type of the material to a unique
identifier that will be used after that to automatically generate different instances of the material. The implementation
of the interface Ra::Core::Asset::MaterialData can add whatever functions needed to construct and interact with the external
representation of a material. These functions might then be used by the file loader able to understand this material
and by the material converter, described below, that will be used by some systems to convert this external material
definition to the Engine internal representation.

### The Material interface
The Ra::Engine::Material interface defines the internal abstract representation of a Material. This interface will be used by
the Engine, mainly by the Ra::Engine::RenderTechnique and the Ra::Engine::Renderer.

This interface defines all the methods needed to parametrized the OpenGL pipeline for rendering.
When implementing this interface, it is a good idea to add two static methods to the implementation to allow to register
and unregister the material into the Engine.
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

### Material converters
This is used by the loading system to translate the external representation of a material to the internal one and
associate this internal representation to a Ra::Engine::RenderTechnique inside a Ra::Engine::RenderObjectcomponent.

A material converter is a couple `<std::string, std::function<Ra::Engine::Material*(Ra::Asset::MaterialData*)>>`
where the string gives the type of the material and the function is whatever is compatible with std::function :
 - a lambda
 - a functor
 - a function with bind parameters ...

The function is in charge of converting a concrete `Ra::Core::Asset::MaterialData *` to a concrete
`Ra::Engine::Material *` according to the type of material described by the string ...

Material converters are managed by the engine through a Factory defined in the `namespace Ra::Engine::EngineMaterialConverters`
and located in the `Engine/Renderer/Material` directory as below :
~~~{.cpp}
namespace EngineMaterialConverters {
using AssetMaterialPtr = const Ra::Asset::MaterialData*;
using RadiumMaterialPtr = Ra::Engine::Material*;
using ConverterFunction = std::function< RadiumMaterialPtr( AssetMaterialPtr ) >;

/** register a new material converter */
RA_ENGINE_API bool registerMaterialConverter( const std::string& name,
                                              ConverterFunction converter );

/** remove a material converter */
RA_ENGINE_API bool removeMaterialConverter( const std::string& name );

/** Get a material converter */
RA_ENGINE_API std::pair<bool, ConverterFunction> getMaterialConverter( const std::string& name );

} // namespace EngineMaterialConverters
~~~

### Render technique and materials
A Ra::Engine::RenderTechnique correspond to the description of how to use Materials to render an object in openGL.
Even if Ra::Engine::RenderTechnique is tightly coupled with the default Ra::Engine::ForwardRenderer of the engine, it could
be used also with others renderer. Note nevertheless that RenderTechnique is not mandatory when defining a specific
renderer as the association between the material and the geometry of a render object could be done explicitly.

To manage the way a Material could be used for rendering, a Ra::Engine::RenderTechnique is then a set of
_shader configurations_ associated to the different way a renderer will compute the final image.
Based on the Ra::Engine::ForwardRenderer implementation in Radium, the set of configurations, with one configuration per
rendering _passes_ corresponds to the following :
 1. Z-prepass : depth and ambient/environment lighting :
    - Identified by the `Ra::Engine::RenderTechnique::Z_PREPASS` constant.
    - Required for the depth pre-pass of several renderer.
    - Must initialise the color buffer with the computation of ambient/environment lighting.
    - Must discard all non fully opaque fragments.
    - Default/Reference : ``Material/BlinnPhong/DepthAmbientBlinnPhong` shader
 2. Opaque lighting **(MANDATORY for default ForwardRenderer)**:
    - Identified by the `Ra::Engine::RenderTechnique::LIGHTING_OPAQUE` constant.
    - Main configuration, computes the resulting color according to a lighting configuration.
    - The lighting configuration might contains one or several sources of different types.
    - Must discard all non fully opaque fragments.
    - Default/Reference : BlinnPhong shader
 3. Transparent lighting :
   - Identified by the `Ra::Engine::RenderTechnique::LIGHTING_TRANSPARENT` constant.
   - Must discard fully transparent and fully opaque fragments, Others will be lit and blended
   according to the algorithm described in
       - Weighted Blended Order-Independent Transparency
         Morgan McGuire, Louis Bavoil - NVIDIA
         Journal of Computer Graphics Techniques (JCGT), vol. 2, no. 2, 122-141, 2013
         http://jcgt.org/published/0002/02/09/
   - Lighting is computed the same way as for Opaque Lighting
   - Default/Reference : ``Material/BlinnPhong/LitOITBlinnPhong`` shader
   - The transparent color weighting function might be the same as :
 ~~~{.cpp}
float weight(float z, float alpha) {
    // pow(alpha, colorResistance) : increase colorResistance if foreground transparent are affecting background
    //                               transparent color
    // clamp(adjust / f(z), min, max) :
    //     adjust : Range adjustment to avoid saturating at the clamp bounds
    //     clamp bounds : to be tuned to avoid over or underflow of the reveleage texture.
    // f(z) = 1e-5 + pow(z/depthRange, orederingStrength)
    //     defRange : Depth range over which significant ordering discrimination is required.
    //             Here, 10 image space units.
    //         Decrease if high-opacity surfaces seem “too transparent”,
    //         increase if distant transparents are blending together too much.
    //     orderingStrength : Ordering strength. Increase if background is showing through foreground too much.
    // 1e-5 + ... : avoid dividing by zero !

    return pow(alpha, 0.5) * clamp(10 / ( 1e-5 + pow(z/10, 6)  ), 1e-2, 3*1e3);
}
~~~

**Note** that a specific renderer might use the same set of configurations but with a different semantic.
One can imagine, for instance, that a renderer will only use the _Depth and ambient/environment_ configuration in order
to render an object without light source but with a specific color computation.


RenderTechniques are associated with Materials through a Builder Factory defined in the `namespace Ra::Engine::EngineRenderTechniques`
and located in the `Engine/Renderer/RenderTechnique` directory.
This factory will manage default technique builders for each registered materials in the engine.
A default technique builder will associate a set of predefined shader for each rendering pass to a Material type.

This association is based on the type, not on the instance. So it can vary from one instance to the other but
requires then a manual construction of the render technique instead of an automatic one through the factory.

The Ra::Engine::RenderTechnique factory is defined as below :
~~~{.cpp}
namespace EngineRenderTechniques {
using DefaultTechniqueBuilder = std::function<void( RenderTechnique&, bool )>;

/** register a new default technique builder for a Material Type*/
RA_ENGINE_API bool registerDefaultTechnique( const std::string& name, DefaultTechniqueBuilder builder );

/** Remove a default technique builder */
RA_ENGINE_API bool removeDefaultTechnique( const std::string& name );

/** Get the default technique builder for a Material Type */
RA_ENGINE_API std::pair<bool, DefaultTechniqueBuilder> getDefaultTechnique( const std::string& name );

} // namespace EngineRenderTechniques
~~~

**Note** that, if needed, an application could bypass the builder factory and construct directly a render technique.
More, as the material is associated to the RenderObject component and not to the Render technique, an application
 with its own specific renderer could not use at all this functionality.

## Engine material management workflow
For now (master v1), the engine manage only one default material corresponding the the Blinn-Phong BSDF.
The type of this material is Ra::Engine::BlinnPhongMaterial.
The workflow allowing the Engine to manage this material is the following.

### Making BlinnPhong a loadable material (see _The MaterialData interface_)
This part of the Material management workflow is related to File loader. So, The corresponding classes are located in
the `Core/File` subdirectory.
~~~
src
└───Core
│   └───File
│       │   ...
│       │   BlinnPhongMaterialData.cpp
│       │   BlinnPhongMaterialData.hpp
│       │   BlinnPhongMaterialData.inl
│       │   ...
│
...
~~~
The Ra::Core::Asset::BlinnPhongMaterialData class is defined as :
~~~{.cpp}
namespace Ra {
namespace Asset {

class RA_CORE_API BlinnPhongMaterialData : public Ra::Engine::MaterialData {
public:
    explicit BlinnPhongMaterialData( const std::string& name = "" );
    ...
};

} // namespace Asset
} // namespace Ra
~~~

Then, Ra::IO::AssimpFileLoader instantiates the
Ra::Core::Asset::BlinnPhongMaterialData when loading a file in the following way :
~~~{.cpp}
void AssimpGeometryDataLoader::loadMaterial( const aiMaterial& material,
                                             Ra::Core::Asset::GeometryData& data ) const {
    // Get the name of the material
    std::string matName;
    aiString assimpName;
    if ( AI_SUCCESS == material.Get( AI_MATKEY_NAME, assimpName ) )
    {
        matName = assimpName.C_Str();
    }
    // Instanciate a BlinnPhongMaterialData object
    auto blinnPhongMaterial = new Asset::BlinnPhongMaterialData( matName );
    // Fill the BlinnPhongMaterialData structure
    ...
    // Associate the Material with the geometry
    data.setMaterial( blinnPhongMaterial );
}
~~~

### Making BlinnPhong a usable material (see Ra::Engine::Material)
This part of the Material management workflow is related to the Renderer part of the Engine. So, The corresponding classes are located in
the `Engine/Renderer/Material` subdirectory.
~~~
src
└───Engine
│   └───Renderer
│   │   └───Material
│   │   │   ...
│   │   │   BlinnPhongMaterial.cpp
│   │   │   BlinnPhongMaterial.hpp
│   │   │   BlinnPhongMaterial.inl
│   │   │   ...
│   │   ...
│   ...
...
~~~
The Ra::Engine::BlinnPhongMaterial class is defined as :
~~~{.cpp}
namespace Ra {
namespace Engine {

class RA_ENGINE_API BlinnPhongMaterial final : public Ra::Engine::Material {
  public:
    explicit BlinnPhongMaterial( const std::string& name );
    ...
  private:
    ...
  public:
    static void registerMaterial();
    static void unregisterMaterial();
    ...

};

} // namespace Engine
} // namespace Ra
~~~

### Registering Ra::Engine::BlinnPhongMaterial to the Ra::Engine::RadiumEngine
The registration (and unregistering) of a Ra::Engine::Material into the Ra::Engine::RadiumEngine consists in registering the material converter into
the MaterialConvertersFactory and the RenderTechniqueBuilder into the RenderTechniqueFactory.

It is recommended (see above) to implement specific class methods in the Material implementations that will register
and unregister the material type into the engine.

For the default Ra::Engine::BlinnPhongMaterial, which is of type `"BlinnPhong"`, these methods will do the following :

~~~{.cpp}
void BlinnPhongMaterial::registerMaterial() {
    // For internal resources management in a filesystem
    std::string resourcesRootDir = {Core::Resources::getRadiumResourcesDir()};
    // 1- register the Material converter for loading
    EngineMaterialConverters::registerMaterialConverter(
        "BlinnPhong",
        BlinnPhongMaterialConverter() );

    // 2- register the technique builder for rendering
    Ra::Engine::EngineRenderTechniques::registerDefaultTechnique(
        "BlinnPhong",
        []( Ra::Engine::RenderTechnique& rt, bool isTransparent ) {
            // Configuration for RenderTechnique::LIGHTING_OPAQUE (Mandatory)
            Ra::Engine::ShaderConfiguration lpconfig(
                "BlinnPhong", resourcesRootDir+"Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                resourcesRootDir+"Shaders/Materials/BlinnPhong/BlinnPhong.frag.glsl" );
            Ra::Engine::ShaderConfigurationFactory::addConfiguration( lpconfig );
            rt.setConfiguration( lpconfig, Ra::Engine::RenderTechnique::LIGHTING_OPAQUE );

            // Configuration for RenderTechnique::Z_PREPASS
            Ra::Engine::ShaderConfiguration dpconfig(
                "DepthAmbiantBlinnPhong", resourcesRootDir+"Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                resourcesRootDir+"Shaders/Materials/BlinnPhong/DepthAmbientBlinnPhong.frag.glsl" );
            Ra::Engine::ShaderConfigurationFactory::addConfiguration( dpconfig );
            rt.setConfiguration( dpconfig, Ra::Engine::RenderTechnique::Z_PREPASS );

            // Configuration for RenderTechnique::LIGHTING_TRANSPARENT
            if ( isTransparent )
            {
                Ra::Engine::ShaderConfiguration tpconfig(
                    "LitOITBlinnPhong", resourcesRootDir+"Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                    resourcesRootDir+"Shaders/Materials/BlinnPhong/LitOITBlinnPhong.frag.glsl" );
                Ra::Engine::ShaderConfigurationFactory::addConfiguration( tpconfig );
                rt.setConfiguration( tpconfig, Ra::Engine::RenderTechnique::LIGHTING_TRANSPARENT );
            }
        } );
}

void BlinnPhongMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( "BlinnPhong" );
    EngineRenderTechniques::removeDefaultTechnique( "BlinnPhong" );
}
~~~

As Ra::Engine::BlinnPhongMaterial is directly managed by the Engine, registration of the material is realized at the
initialization of the Engine.
We then have, in the Ra::Engine::RadiumEngine, the following method :
~~~{.cpp}
void RadiumEngine::initialize() {
    LOG( logINFO ) << "*** Radium Engine ***";
    ...
    // Engine support some built-in materials. Register them here
    BlinnPhongMaterial::registerMaterial();
}
~~~

### Asssociate the material with a RenderTechnique inside a RenderObject
The conversion of a Ra::Core::Asset::FileData structure, resulting from the loading of a data file to entities and components that
are managed by the Engine and Renderers are in charge of the loading system, by default the `FancyMesh` system.

Thanks to the Ra::Core::Asset::MaterialData interface, the Ra::Engine::Material interface, the `EngineMaterialConverters` factory
and the Ra::Engine:::RenderTechnique factory, this task is generic and is the same for every registered materials.
So, once a material is developed following the above description, is will be directly usable in the Ra::Engine::ForwardRenderer
thanks to the following Ra::Engine::TriangleMeshComponent method :

~~~{.cpp}
void TriangleMeshComponent::handleMeshLoading( const Ra::Asset::GeometryData* data ) {
    // convert the GeometryData into a Renderable Mesh
    ...
    // Convert the MaterialData into a Material and its associated render technique

    // First, get the concrete MaterialData
    const Ra::Asset::MaterialData& loadedMaterial = data->getMaterial();
    // Second, convert the material from asset to Engine
    auto converter =
            Ra::Engine::EngineMaterialConverters::getMaterialConverter( loadedMaterial.getType() );
    std::shared_ptr<Ra::Engine::Material> radiumMaterial( converter.second( &loadedMaterial ) );
    // Third, associate the material to a render technique
    Ra::Engine::RenderTechnique rt;
    rt.setMaterial( radiumMaterial );
    // Fourth, parameterize the technique for rendering this material
    auto builder =
            Ra::Engine::EngineRenderTechniques::getDefaultTechnique( loadedMaterial.getType() );
    builder.second( rt, isTransparent );

    // Create, parameterize and register the resulting  RenderObject
    auto ro = Ra::Engine::RenderObject::createRenderObject(
        roName, this, Ra::Engine::RenderObjectType::Fancy, displayMesh, rt );
    ro->setTransparent( radiumMaterial->isTransparent() );
    setupIO( m_contentName );
    m_meshIndex = addRenderObject( ro );
}
~~~

## Extending the material library from a plugin

Note that, as we will see in the _Extending the material library from a plugin_ if one want to add a new loadable
material in the Engine, this will require to define a loader capable of generating such data.

Given the description above, one can extend the material library from a plugin or from an application.
