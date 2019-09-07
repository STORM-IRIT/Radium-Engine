# Geometry and meshes {#geometry}

## Mesh types
There is three types of meshes included in radium source :
1. `TriangleMesh`, which handles geometry data and connectivity as a indexed vertex array. 
Each vertex is a unique set of position, normal, and every attribute.
Faces are defined using two vectors of indices, one for triangles and one for other types of polygon.
This class handles data only, in a way suitable for rendering.
2. `TopologicalMesh`, which is an half-edge data structure. A converter allows to go back and forth to `TriangleMesh` 
without loss of data, but during the conversion, vertices with the same position represents the same topological point 
(and are hence merged). The other vertex attributes are stored on half-edges (to manage multiple normals per 3D positions 
for instance).
3. `Mesh`, which stores a `TriangleMesh` to handle 3D data, and manages the rendering aspect of it (VAO, VBO, draw call).


## Mesh creation
`GeometryComponent` is in charge of loading a `GeometryData` and create the corresponding `Mesh`.

The user can add on the fly new vertex attributes to a `TriangleMesh`.
An attribute is defined by a unique name (`std::string`) and then represented as a `AttribHandle<T>`.
The type of the attribute is defined by the caller using the template parameter of the method `addAttrib`.
To ensure consistency, it is strongly recommended to store the handle returned by the `addAttrib` method:
```
        auto handle1 = m.addAttrib<Vector3>( "vector3_attrib" );
```
Attribute names `in_position` and `in_normals` are reserved.
Note that handles to existing attributes can be retrieved by name (see `getAttrib(const std::string& name)`), however 
the caller have to provide the type of the associate attribute. 
There is no type check on this access, and we recommend not to use this. We keep this method for convenience in some 
specific cases such as copy of attributes, however note that it might be marked as deprecated at any time.

There is no (in the current version) data checking, and attributes are simple `std::vector`s added to the attribute 
manager. It's meant to be used as vector of data, such as vertices and normals, with the same size and accessed with 
the vertices indices. But please be aware no check nor allocation are done, so the allocation has to be performed on 
client side.

When copying a `TriangleMesh`, all its attributes are copied, unless it is done through `copyBaseGeometry`, for which 
no attribute is copied.
In order to copy some/all of the attributes, the dedicated methods must be used.

For instance
```c++
        TriangleMesh m;
        m.vertices().push_back( {0, 0, 0} );
        m.vertices().push_back( {1, 0, 0} );
        m.vertices().push_back( {0, 2, 0} );
        m.normals().push_back( {0, 0, 1} );
        m.normals().push_back( {0, 0, 1} );
        m.normals().push_back( {0, 0, 1} );
        m.m_triangles.push_back( {0, 1, 2} );

        auto handle1 = m.addAttrib<Vector3>( "vector3_attrib" );
        auto& buf = m.getAttrib( handle1 ).data();
        buf.reserve( 3 );
        buf.push_back( {1, 1, 1} );
        buf.push_back( {2, 2, 2} );
        buf.push_back( {3, 3, 3} );

        AttribHandle<float> handle2 = m.addAttrib<float>( "float_attrib" );
        auto attrib2 = m.getAttrib( handle2 );
        attrib2.data().assign( { 1.f, 2.f, 3.f } );
        
        TriangleMesh m2;
        m2.copyBaseGeometry( m );
        m2.copyAttributes( m, handle1 );
```

