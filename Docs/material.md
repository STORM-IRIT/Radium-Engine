# Material management in the Radium Engine
A Material is a way to control the appearance of an object when rendering. It could be The definition of a classical 
rendering materials, a _Bidirectionnal Scattering Distribution function (BSDF)_, or just define the way a geometry 
could be rendered and how is computed the final color of an object.

A material is associated to the renderable geometry of an object (a component in the Radium nomenclature) through a so 
called _Render Technique_. This association is managed by the ```RenderObject``` class.

This documentation aims at describing the way materials are managed in the Radium engine and how one can extend the set 
of available material, or specialized a renderer with ad-hoc (pseudo)-material.

## Material subsystem and Material Library
The Radium Engine defines an abstract interface for material definition that allows easy extensibility of the Engine.
This section describe the workflow of Material management from asset loading to rendering. Note that, for a given 
application or renderer, a Material could be defined directly without loading it from a file.

### The MaterialData interface
The interface ```MaterialData``` define the external representation of a material. Even if this interface could be 
instanciated, it defines an abstract material that is not valid for the Engine. 
This interface must then be implemented to define materials that could be loaded from a file.

When defining a loadable material, the corresponding implementation must set the type of the material to a unique 
identifier that will be used after that to automatically generate different instances of the material. The implementation
of the interface ```MaterialData``` can add whatever functions needed to construct and interact with the external 
representation of a material. These functions might then be used by the file loader able to understand this material 
and by the material converter, described below, that will be used by some systems to convert this external material 
definition to the Engine internal representation.

### The Material interface
The ```Material``` interface defines the internal abstract representation of a Material. This interface will be used by 
the Engine, mainly by the _Render Technique_ and the renderers.

This interface defines all the methods needed to parameterize the OpenGL pipelline for rendering.
When implementing this interface, it is a good idea to add two static methods to the implementation to allow to register 
and unregister the material into the Engine.
These method could have the following profiles :
```cpp
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
``` 

### Material converters
This is used by the loading system to translate the external representation of a material to the internal one and 
associate this internal representation to a ```RenderTechnique``` inside a ```RenderObject```component.

A material converter is a couple ```<std::string, std::function<Ra::Engine::Material*(Ra::Asset::MaterialData*)>``` 
where the string gives the type of the material and the function is whatever is compatible with std::function :
- a lambda
- a functor
- a function with bind parameters ...

The function is in charge of converting a concrete ```Ra::Asset::MaterialData*``` to a concrete
```Ra::Engine::Material*``` according to the type of material described by the string ...

Material converters are managed by the engine through a Factory defined in the ```namespace Ra::Engine::EngineMaterialConverters```
and located in the ```Engine/Renderer/Material``` directory as below :
```cpp
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
``` 

### Render technique and materials
A ```RenderTechnique``` correspond to the description of how to use Materials to render an object in openGL.
Even if ```RenderTechnique``` is tightly coupled with the default ```ForwardRenderer``` of the engine, it could 
be used also with other renderers. Note nevertheless that RenderTechnique is not mandatory when defining a specific 
renderer as the association between the material and the geometry of a render object could be done explicitely.

To manage the way a Material could be used for rendering, a ```RenderTechnique``` is then a set of 
_shader configurations_ associated to the different way a renderer will compute the final image. 
Based on the ```ForwardRenderer``` implementation in Radium, the set of configurations, with one configuration per 
rendering _passes_ corresponds to the following :
1. Depth and ambiant/environment lighting :
    - Identified by the ```RenderTechnique::Z_PREPASS``` constant.
    - Required for the depth pre-pass of several renderers.
    - Must initialise the color buffer with the computation of ambiant/environment lighting. 
    - Default/Reference : DepthAmbiantPass shaders
2. Opaque lighting **(MANDATORY for default ForwardRenderer)**: 
    - Identified by the ```RenderTechnique::LIGHTING_OPAQUE``` constant.
    - Main configuration, computes the resulting color according to a lighting configuration.
    - The lighting configuration might contains one or several sources of different types.
    - Default/Reference : BlinnPhong shaders
3. Transparent lighting :
    - Identified by the ```RenderTechnique::LIGHTING_TRANSPARENT``` constant.
    - Same as opaque lighting but for transparent objects
    - Default/Reference LitOIT shaders

**Note** that a specific renderer might use the same set of configurations but with a different semantic. 
One can imagine, for instance, that a renderer will only use the _Depth and ambiant/environment_ configuration in order 
to render an object without light source but with a specific color computation.


RenderTechniques are associated with Materials through a Builder Factory defined in the ```namespace Ra::Engine::EngineRenderTechniques```
and located in the ```Engine/Renderer/RenderTechnique``` directory.
This factory will manage default technique builders for each registered materials in the engine. 
A default technique builder will associate a set of predefined shaders for each rendering pass to a Material type. 

This association is based on the type, not on the instance. So it can vary from one instance to the other but
requires then a manual construction of the render technique instead of an automatic one through the factory.

The ```RenderTechnique``` factory is defined as below :
```cpp
namespace EngineRenderTechniques {
using DefaultTechniqueBuilder = std::function<void( RenderTechnique&, bool )>;

/** register a new default technique builder for a Material Type*/
RA_ENGINE_API bool registerDefaultTechnique( const std::string& name, DefaultTechniqueBuilder builder );

/** Remove a default technique builder */
RA_ENGINE_API bool removeDefaultTechnique( const std::string& name );

/** Get the default technique builder for a Material Type */
RA_ENGINE_API std::pair<bool, DefaultTechniqueBuilder> getDefaultTechnique( const std::string& name );

} // namespace EngineRenderTechniques
``` 

**Note** that, if needed, an application could bypass the builder factory and construct directly a render technique. 
More, as the material is associated to the RenderObject component and not to the Render technique, an application 
 with its own specifi renderer could not use at all this functionality.

## Engine material management workflow
For now (master v1), the engine manage only one default material corresponding the the Blinn-Phong BSDF. 
The type of this material is ```"BlinnPhong"```. 
The workflow allowing the Engine to manage this material is the following. 

### Making BlinnPhong a lodable material (see _The MaterialData interface_)
This part of the Material management workflow is related to File loader. So, The corresponding classes are located in 
the ```Core/File``` subdirectory.
```
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
```
The ```BlinnPhongMaterialData``` class is defined as :
```cpp
namespace Ra { 
namespace Asset {

class RA_CORE_API BlinnPhongMaterialData : public MaterialData {
public:
    explicit BlinnPhongMaterialData( const std::string& name = "" );
    ...
};
    
} // namespace Asset
} // namespace Ra
```

Then, the Assimp loader, located in the ```IO/AssimpLoader``` subdirectory will instanciate the 
```BlinnPhongMaterialData``` when loading a file in the following way :
```cpp
void AssimpGeometryDataLoader::loadMaterial( const aiMaterial& material,
                                             Asset::GeometryData& data ) const {
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
```

### Making BlinnPhong a renderable material (see _The Material interface_)
This part of the Material management workflow is related to the Renderer part of the Engine. So, The corresponding classes are located in 
the ```Engine/Renderer/Material``` subdirectory.
```
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
```
The ```BlinnPhongMaterial``` class is defined as :
```cpp
namespace Ra {
namespace Engine {

class RA_ENGINE_API BlinnPhongMaterial final : public Material {
  public:
    RA_CORE_ALIGNED_NEW
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
```

### Registering the BlinnPhongMaterial to the Engine
The registration (and unregistration) of a Material into the Engine consists in registering the material converter into
the MaterialConvertersFactory and the RenderTechniqueBuilder into the RenderTechniqueFactory.

It is recommanded (see above) to implement specific class methods in the Material implementations that will register 
and unregister the material type into the engine.

For the default ```BlinnPhongMaterial```, wich is of type ```"BlinnPhong"```, these methods will do the following :

```cpp
void BlinnPhongMaterial::registerMaterial() {
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
                "BlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                "Shaders/Materials/BlinnPhong/BlinnPhong.frag.glsl" );
            Ra::Engine::ShaderConfigurationFactory::addConfiguration( lpconfig );
            rt.setConfiguration( lpconfig, Ra::Engine::RenderTechnique::LIGHTING_OPAQUE );

            // Configuration for RenderTechnique::Z_PREPASS
            Ra::Engine::ShaderConfiguration dpconfig(
                "DepthAmbiantBlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                "Shaders/Materials/BlinnPhong/DepthAmbientBlinnPhong.frag.glsl" );
            Ra::Engine::ShaderConfigurationFactory::addConfiguration( dpconfig );
            rt.setConfiguration( dpconfig, Ra::Engine::RenderTechnique::Z_PREPASS );
            
            // Configuration for RenderTechnique::LIGHTING_TRANSPARENT
            if ( isTransparent )
            {
                Ra::Engine::ShaderConfiguration tpconfig(
                    "LitOITBlinnPhong", "Shaders/Materials/BlinnPhong/BlinnPhong.vert.glsl",
                    "Shaders/Materials/BlinnPhong/LitOITBlinnPhong.frag.glsl" );
                Ra::Engine::ShaderConfigurationFactory::addConfiguration( tpconfig );
                rt.setConfiguration( tpconfig, Ra::Engine::RenderTechnique::LIGHTING_TRANSPARENT );
            }
        } );
}

void BlinnPhongMaterial::unregisterMaterial() {
    EngineMaterialConverters::removeMaterialConverter( "BlinnPhong" );
    EngineRenderTechniques::removeDefaultTechnique( "BlinnPhong" );
}
``` 

As the ```BlinnPhongMaterial``` is directly managed by the Engine, registration of the material is realized at the 
initialization of the Engine.
We then have, in the ```RadiumEngine``` class, the following method :
```cpp
void RadiumEngine::initialize() {
    LOG( logINFO ) << "*** Radium Engine ***";
    ...
    // Engine support some built-in materials. Register them here
    BlinnPhongMaterial::registerMaterial();
}
```

### Asssociate the material with a RenderTechnique inside a RenderObject
The conversion of a ```FileData``` structure, resulting from the loading of a data file to entities and components that
are managed by the Engine and Renderers are in charge of the loading system, by default the ```FancyMesh``` system. 

Thanks to the ```MaterialData``` interface, the ```Material``` interface, the ```EngineMaterialConverters``` factory 
and the ```EngineRenderTechniquesFactory```, this task is generic and is the same for every registered materials.
So, once a material is developped following the above description, is will be directly usable in the ````orwardRenderer```
thanks to the followinf ```FancyMesh```method :

```cpp
void FancyMeshComponent::handleMeshLoading( const Ra::Asset::GeometryData* data ) {
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
```

## Extending the material library from a plugin

Note that, as we will see in the _Extending the material library from a plugin_ if one want to add a new loadable 
material in the Engine, this will require to define a loader capable of generating such data.

Given the description above, one can extend the material library from a plugin or from an application.

