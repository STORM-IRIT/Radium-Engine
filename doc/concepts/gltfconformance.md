\page gltfConformance GLTF 2.0 support status
[TOC]

# GLTF (2.0) support for Radium engine

This document indicates, from the main structures defined in the GLTF2.0 specification, what is supported,
partly supported or not supported.

For more information about gltf file format and its use by 3D engines/apps visit https://www.khronos.org/gltf/

## GLTF2.0 coverage

The implementation of the gltf specification is tested (interactively and visually) using several _official_ gltf
sample models available at https://github.com/KhronosGroup/glTF-Sample-Models.

The structure of this document is the same than those of the
[official specification](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0)

* [GLTF 2.0 elements](#gltf_elements)
  * [Asset](#asset)
  * [Indices and Names](#indices-and-names)
  * [Coordinate System and Units](#coordinate-system-and-units)
  * [Scenes](#scenes)
    * [Nodes and Hierarchy](#nodes-and-hierarchy)
    * [Transformations](#transformations)
  * [Binary Data Storage](#binary-data-storage)
    * [Buffers and Buffer Views](#buffers-and-buffer-views)
      * [GLB-stored Buffer](#glb-stored-buffer)
    * [Accessors](#accessors)
      * [Accessors Bounds](#accessors-bounds)
      * [Sparse Accessors](#sparse-accessors)
    * [Data Alignment](#data-alignment)
  * [Geometry](#geometry)
    * [Meshes](#meshes)
      * [Tangent-space definition](#tangent-space-definition)
      * [Morph Targets](#morph-targets)
    * [Skins](#skins)
      * [Skinned Mesh Attributes](#skinned-mesh-attributes)
      * [Joint Hierarchy](#joint-hierarchy)
    * [Instantiation](#instantiation)
  * [Texture Data](#texture-data)
    * [Textures](#textures)
    * [Images](#images)
    * [Samplers](#samplers)
  * [Materials](#materials)
    * [Metallic-Roughness Material](#metallic-roughness-material)
    * [Additional Maps](#additional-maps)
    * [Alpha Coverage](#alpha-coverage)
    * [Double Sided](#double-sided)
    * [Default Material](#default-material)
    * [Point and Line Materials](#point-and-line-materials)
  * [Cameras](#cameras)
    * [Projection Matrices](#projection-matrices)
  * [Animations](#animations)
* [Specifying Extensions](#extensions)
  * [Supported extensions](#supported-extensions)
  * [Unsupported extensions](#unsupported-extensions)

## GLTF 2.0 elements {#gltf_elements}

### Asset {#asset}

The `asset` node ([specification](https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#reference-asset))
is parsed when loading a glTF file and its content is logged in the Radium `LogINFO` logger.

### Indices and Names {#indices-and-names}

Names of glTF entities are transmitted to the Radium Engine. These names then serve as keys for several resources management components.

### Coordinate System and Units {#coordinate-system-and-units}

glTF uses a right-handed coordinate system that correspond to the coordinate system of Radium.
The test [Boom with axes](https://github.com/KhronosGroup/glTF-Sample-Models/tree/master/2.0/BoomBoxWithAxes) pass.

### Scenes {#scenes}

All the scenes are loaded but only the one referenced by the `scene` property is converted to Radium asset.
If `scene`is undefined, the scene `0` is converted.

#### Nodes and Hierarchy {#nodes-and-hierarchy}

After loading, the node hierarchy defined in the glTF file is flattened.

> In a future version of Radium, node hierarchy should be maintained.

#### Transformations {#transformations}

All the glTF transformations are integrated when flattening the nodes tree.

### Binary Data Storage {#binary-data-storage}

#### Buffers and Buffer Views {#buffers-and-buffer-views}

All buffers are loaded when parsing a glTF file.
Buffer views will be used to extract data from buffers and fill Radium IO data structure.
The `target` property of a buffer is not used by the loader that infer usage of the buffer from the mesh `accessor` properties.

> the `byteStride` property, defined only for buffer views that contain vertex attributes, is not used in the converter.
Even if all supported sample models loads well (as well as sketchfab downloaded models) this should be taken it into
account in the near future.

##### GLB-stored Buffer {#glb-stored-buffer}

GLB-stored buffer are transparently managed by the loader.

#### Accessors {#accessors}

Accessors are used to build `RadiumIO` compatible description of the Geometry.

##### Accessors Bounds {#accessors-bounds}

The `min`and `max`properties are not used as they are recomputed by Radium when building a mesh.

##### Sparse Accessors {#sparse-accessors}

Not tested.

##### Data Alignment {#data-alignment}

For the moment, data are considered as non aligned.

> Should take into account alignment to handle all the test cases.

### Geometry {#geometry}

Only non morphed meshes are managed.
The loader read all from the file but the Radium converter ignores morphing based animation targets.

#### Meshes {#meshes}

The way a mesh is divided into `primitives` is kept while building radium geometries.
Each primitive defines a Radium mesh on its own.

Due to restriction in Radium, only the texture coord layer 0 (`TEXCOORD_0`) is converted and used.

> In the next version of Radium, both `TEXCOORD_0` and `TEXCOORD_1` should be made available.

The `COLOR_0` property  is not used for now.

> In a near future, this should be OK as Radium manage color attributes per vertex.

Weighs and joints are correctly loaded and transmitted to Radium for skeleton based animation.

> Du to limitations in the Radium animation system, regular nodes and nodes hierarchies attached to a joint are not
handled the way it is requested by the gtlf specification.

If normals are not given, they are computed, as well as tangent, according to the GLTF specification.

##### Tangent-space definition {#tangent-space-definition}

If tangent are not defined, the loader use the [MikkTSpace algorithm](http://image.diku.dk/projects/media/morten.mikkelsen.08.pdf),
in its original implementation by the author to compute the tangents. This respect the GLTF specification.

##### Morph Targets {#morph-targets}

Not yet managed by the converter.

#### Skins {#skins}

Skins are loaded and skeletons are build according to the specification but with the restriction imposed by Radium
for now (one single mesh per skeleton, no node hierarchy attached to a joint, ...)

### Texture Data {#texture-data}

glTF separates texture access into three distinct types of objects: Textures, Images, and Samplers.
For the moment, Radium only represents textures by a single type of object.
Several conformance tests, where the same image is used with different samplers, do not pass yet.

> In an upcomming Radium PR, textures should be managed as an association of images and samplers.

#### Images {#images}

Only images defined by a URI to an external file are managed.

> In the future, allow to build textures from already defined data instead of an external file.
Also allow to generate procedural textures (needed by some glTF extensions)

#### Samplers {#samplers}

Sampler are used to parameterize the Radium textures. Due to limitations in texture management in Radium, there are
some inconsistencies in the sampler management.
When a texture is referenced through several samplers, only the first will be used for all the instances.

### Materials {#materials}

Physically based rendering is supported in Radium. glTF materials are fully supported if they belong to the core specification.
Adding support for other materials might be done easily as soon as there specification and json schema are known.

#### Metallic-Roughness Material {#metallic-roughness-material}

This material is integrated in the glTF plugin.

#### Additional Maps {#additional-maps}

The main texture maps defined by the specification are integrated.

#### Alpha Coverage {#alpha-coverage}

Alpha coverage test succeed as soon as there is no sampler collision (see above)

#### Double Sided {#double-sided}

Double sided materials are manage directly in the shader by discarding (or not) fragments that are back-side.

#### Point and Line Materials {#point-and-line-materials}

Not yet supported.

### Cameras {#cameras}

Camera are loaded and transmitted to Radium.

#### Projection Matrices {#projection-matrices}

Both perspective and orthogonal projection matrices are managed

### Animations {#animations}

Animation key-framed definition is properly loaded and transmitted to Radium. Note that all interpolators are transmitted to Radium as the linear one.

> In a next version, support all the key-frames interpolators.

## Extensions {#extensions}

glTF defines an extension mechanism that allows the base format to be extended with new capabilities.
Some extensions are ratified by Khronos Groups (__KHR_extensions__), others are proposed by vendors (Adobe, nvidia, microsoft, ...).
Radium GLTF will integrate __KHR__ extensions as much as possible.
Bellow is the list of supported, WIP and unsupported extensions in RadiumGLTF.

### Supported extensions {#supported-extensions}

* __KHR_materials_pbrSpecularGlossiness__, defines a PBR material based on specular color and
  shininess exponent is supported (spec at https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_pbrSpecularGlossiness).

* __KHR_texture_transform__, allows to apply transformation on texture coordinate is supported
  (spec at https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_texture_transform)

* __KHR_lights_punctual__, defines three "punctual" light types: directional, point and spot.
Punctual lights are defined as parameterized, infinitely small points that emit light in well-defined directions and
intensities. (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_lights_punctual)

* __KHR_materials_ior__, allows users to set the index of refraction of a material.
(spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_ior)

* __KHR_materials_clearcoat__, represents a protective layer applied to a base material.
(spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_clearcoat)

* __KHR_materials_specular__, allows users to configure the strength of the specular reflection in the dielectric BRDF.
(spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_specular)

* __KHR_materials_sheen__, defines a sheen that can be layered on top of an existing glTF material definition. A sheen
layer is a common technique used in Physically-Based Rendering to represent cloth and fabric materials
(spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_sheen)

* __KHR_materials_unlit__, defines an unlit shading model for use in glTF 2.0 materials, as an alternative to the
Physically Based Rendering (PBR) shading models provided by the core specification. Three motivating uses cases for
unlit materials include:
  * Mobile devices with limited resources, where unlit materials offer a performant alternative to higher-quality
shading models.
  * Photogrammetry, in which lighting information is already present and additional lighting should not be applied.
  * Stylized materials (such as "anime" or "hand-drawn" looks) in which lighting is undesirable for aesthetic reasons.

  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_unlit)

### Unsupported extensions {#unsupported-extensions}

* __KHR_draco_mesh_compression__, defines a schema to use Draco geometry compression (non-normative) libraries in glTF format.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_draco_mesh_compression)

* __KHR_materials_anisotropy (WIP)__, defines the anisotropic property of a material as observable with brushed metals for example.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_anisotropy)

* __KHR_materials_emissive_strength (WIP)__, defines emissiveStrength scalar factor, that governs the upper limit of emissive
strength per material.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_emissive_strength)

* __KHR_materials_iridescence (WIP)__, describes an effect where hue varies depending on the viewing angle and illumination angle.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_iridescence)

* __KHR_materials_transmission (WIP)__, describes transparency in a PBR way.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_transmission)

* __KHR_materials_variants__, allows for a compact glTF representation of multiple material variants of an asset,
structured to allow low-latency switching at runtime.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_variants)

* __KHR_materials_volume__, makes it possible to turn the surface into an interface between volumes. The volume
extension needs to be combined with an extension which allows light to transmit through the surface, e.g.
__KHR_materials_transmission__.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_materials_volume)

* __KHR_mesh_quantization__, expands the set of allowed component types for mesh attribute storage to provide a
memory/precision tradeoff - depending on the application needs, 16-bit or 8-bit storage can be sufficient.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_mesh_quantization)

* __KHR_texture_basisu__, adds the ability to specify textures using KTX v2 images with Basis Universal supercompression.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_texture_basisu)

* __KHR_xmp_json_ld__, adds support for XMP (Extensible Metadata Platform) (ISO 16684-1) metadata to glTF.
  (spec at https://github.com/KhronosGroup/glTF/tree/main/extensions/2.0/Khronos/KHR_xmp_json_ld)
