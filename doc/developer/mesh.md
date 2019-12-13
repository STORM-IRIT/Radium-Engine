\page develmeshes API: 3D objects (a.k.a. Meshes)
[TOC]

## Geometry types
There is three kind of geometry representation included in radium source :
 1. Ra::Core::Geometry::*, which handles geometry data and connectivity as a indexed vertex array.
Each vertex is a unique set of position, normal, and other attributes.
If indexed, faces are defined with VectorXui, where X is 1 for point, 2 for lines, 3 for triangles, and N for polygons.
See inheritance diagram of Ra::Core::Geometry::AbstractGeometry :
   - Ra::Core::Geometry::AttribArrayGeometry
   - Ra::Core::Geometry::IndexedGeometry
   - Ra::Core::Geometry::LineStrip *
   - Ra::Core::Geometry::PointCloud *
   - Ra::Core::Geometry::IndexedPointCloud *
   - Ra::Core::Geometry::LineMesh *
   - Ra::Core::Geometry::TriangleMesh *

 2. Ra::Core::Geometry::TopologicalMesh, which is an half-edge data structure. 
A converter allows to go back and forth to `TriangleMesh`
without loss of data, but during the conversion, vertices with the same position represents the same topological point (and are hence merged). 
The other vertex attributes are stored on half-edges (to manage multiple normals per 3D positions
for instance).

 3. Ra::Engine::*, which stores a Core Geometry to handle 3D data, and manages the rendering aspect of it (VAO, VBO, draw call).
See inheritance digram of Ra::Engine::AttribArrayDisplayable
   - Ra::Engine::Displayable
   - Ra::Engine::AttribArrayDisplayable
   - Ra::Engine::CoreGeometryDisplayable
   - Ra::Engine::PointCloud *
   - Ra::Engine::IndexedGeometry
   - Ra::Engine::IndexedAttribArrayDisplayable
   - Ra::Engine::LineMesh *
   - Ra::Engine::Mesh *
   
 `*` : the starred classes are the one you want to instanciate, the other are more for code factoring or abstraction.
   
## Colaboration between Core and Engine

A Core Geometry can be used on its own.

Engine Geometry must own a Core Geometry, either set at construction, or later with loadGeometry.
The Core Geometry ownership is then transfered to the Engine Geometry, and can be accessed by reference with Ra::Engine::Displayable::getAbstractGeometry or Ra::Engine::CoreGeometryDisplayable::getCoreGeometry


## Data consistency
As soon as a Core Geometry is owned by a Engine Geometry, each data update on the Core Geometry attribute trigger a observator method to mark the corresponding GPU data as dirty.
On the next Ra::Engine::CoreGeometryDisplayable::updateGL, the dirty data will be updated on the GPU.

## Mesh creation
`GeometryComponent` is in charge of loading a `GeometryData` and create the corresponding `Mesh`.

The user can add on the fly new vertex attributes to a Ra::Core::Geometry::AttribArrayGeometry
An attribute is defined by a unique name (std::string) and then represented as a Ra::Core::Utils::AttribHandle.
The type of the attribute is defined by the caller using the template parameter of the method  Ra::Core::Geometry::AttribArrayGeometry::addAttrib.
To ensure consistency, it is strongly recommended to store the handle returned by the Ra::Core::Geometry::AttribArrayGeometry::addAttrib method:
~~~{.cpp}
        auto handle1 = m.addAttrib<Vector3>( "vector3_attrib" );
~~~
Attribute names `in_position` and `in_normals` are reserved.
Note that handles to existing attributes can be retrieved by name (see Ra::Core::Geometry::AttribArrayGeometry::getAttrib), however
the caller have to provide the type of the associate attribute.
There is no type check on this access, and we recommend not to use this. We keep this method for convenience in some
specific cases such as copy of attributes, however note that it might be marked as deprecated at any time.

There is no (in the current version) data checking, and attributes are simple `std::vector`s added to the attribute
manager. It's meant to be used as vector of data, such as vertices and normals, with the same size and accessed with
the vertices indices. But please be aware no check nor allocation are done, so the allocation has to be performed on
client side.

When copying a Ra::Core::Geometry::AttribArrayGeometry or one of its descendent class, all its attributes are copied, unless it is done through Ra::Core::Geometry::AttribArrayGeometry::copyBaseGeometry, for which no attribute is copied.
In order to copy some/all of the attributes, the dedicated methods must be used.

Attribute writes must be with "lock" to ensure data syncronisation.
Utility methods are provided for position and normals :

 - Ra::Core::Geometry::AttribArrayGeometry::setVertices
 - Ra::Core::Geometry::AttribArrayGeometry::verticesWithLock
 - Ra::Core::Geometry::AttribArrayGeometry::verticesUnlock
 - Ra::Core::Geometry::AttribArrayGeometry::setNormals
 - Ra::Core::Geometry::AttribArrayGeometry::normalsWithLock
 - Ra::Core::Geometry::AttribArrayGeometry::normalsUnlock

Others attributes are written with :
 - Ra::Core::Geometry::AttribArrayGeometry::getAttrib to get an attrib handle
 - Ra::Core::Utils::Attrib::setData to set the data directly 
or
 - Ra::Core::Utils::Attrib::getDataWithLock to get a writable reference to the data array
 - Ra::Core::Utils::Attrib::unlock to release the writable reference

For instance

~~~{.cpp}
using Ra::Core::Geometry::TriangleMesh;
TriangleMesh m;
TriangleMesh::PointAttribHandle::Container vertices;
TriangleMesh::NormalAttribHandle::Container normals;
	
vertices.push_back( {0, 0, 0} );
vertices.push_back( {1, 0, 0} );
vertices.push_back( {0, 2, 0} );
normals.push_back( {0, 0, 1} );
normals.push_back( {0, 0, 1} );
normals.push_back( {0, 0, 1} );
	
m.setVertices( std::move( vertices ) );
m.setNormals( std::move( normals ) );
	
m.m_indices.push_back( {0, 1, 2} );
auto handle1  = m.addAttrib<Vector3>( "vector3_attrib" );
auto& attrib1 = m.getAttrib( handle1 );
auto& buf     = attrib1.getDataWithLock();
	
buf.reserve( 3 );
buf.push_back( {1, 1, 1} );
buf.push_back( {2, 2, 2} );
buf.push_back( {3, 3, 3} );
attrib1.unlock();
	
auto handle2  = m.addAttrib<float>( "float_attrib" );
auto& attrib2 = m.getAttrib( handle2 );
attrib2.setData( {1.f, 2.f, 3.f} );
	
TriangleMesh m2;
m2.copyBaseGeometry( m );
m2.copyAttributes( m, handle1 );
~~~


Other examples are provided in DrawPrimitives.cpp
