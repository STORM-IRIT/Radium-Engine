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
identifier that will be used after that automatically generate different instances of the material. The implementation
of the interface ```MaterialData``` can add whatever functions needed to construct and interact with the external 
representation of a material. These functions might then be used by the file loader able to understand this material 
and by the material converter, described below, that will be used by some systems to convert this external material 
definition to the Engine internal representation.

For now (master v1), the engine define only one loadable material corresponding the the Blinn-phong BSDF. 
The type of this material is ```"BlinnPhong"```. 
The external representation of this material is defined in the ```BlinnPhongMaterialData``` class.


### The Material interface
The ```Material``` interface defines the internal abstract representation of a Material. This interface will be used by 
the Engine, mainly by the _Render Technique_ and the renderers.

### Material converters
This is used by FancyMesh to translate the external representation of a material to the internal one.


## Render technique and materials

## Extending the material library from a plugin