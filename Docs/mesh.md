# Radium Engine 3D objects (a.k.a. Meshes)

## Mesh types
There is three types of meshes included in radium source :
1. `TriangleMesh`
2. `TopologicalMesh`
3. `Mesh`

1. `TriangleMesh` handles geometry data and connectivity as a indexed vertex array. 
Each vertices is a unique set of position, normal, and every attrib.
Faces defined with two vector of indices, one for triangles and one for other type of faces.
This class handles data only, in a way suitable for rendering.
2. `TopologicalMesh` is an half-edge data structure, a converter allows to go back and forth to `TriangleMesh` without loss of data, but during the convertion, vertices with the same position represents the same topological point (and are hence merged). The other vertex attribs are stored on halfedges (to manage multiple normals per 3D positions for instance).
3. `Mesh` has a `TriangleMesh` to handle 3D data, and manage the rendering aspect of a `TriangleMesh` (VAO, VBO, draw call).


## Mesh creation
`FancyMesh` is in charge of loading a `GeometryData` and create the corresponding `Mesh`.

The user can add on the fly new attributes to vertex via `VertexAttribManager` (accessible with `Mesh::attribManager()`).
An attrib is defined by an unique name (`std::string`) and then represented as a `VertexAttribHandle<T>`.
To determine the type of the attrib, the template function `addAttrib` need either a base type variable or a defined handle ref.
The base type or the handle ref value_type is then used to deduce the type of the attrib.

There is no (in the current version) data checking, and attribs are simple `std::vector` added to the attrib manager. It's meant to be used as vector of data as vertices and normals, with the same size and accessed with the faces indices. But please be aware no check nor allocation are done, so the allocation has to be performed on client side.

For instance
```
        TriangleMesh m;
        m.vertices().push_back( {0, 0, 0} );
        m.vertices().push_back( {1, 0, 0} );
        m.vertices().push_back( {0, 2, 0} );
        m.normals().push_back( {0, 0, 1} );
        m.normals().push_back( {0, 0, 1} );
        m.normals().push_back( {0, 0, 1} );
        m.m_triangles.push_back( {0, 1, 2} );

        Vector3 dummy;
        auto handle1 = m.attribManager().addAttrib( dummy, "vector3_attrib" );
        m.attribManager().getAttrib( handle1 ).data().push_back( {1, 1, 1} );
        m.attribManager().getAttrib( handle1 ).data().push_back( {2, 2, 2} );
        m.attribManager().getAttrib( handle1 ).data().push_back( {3, 3, 3} );

        VertexAttribHandle<float> handle2;
        m.attribManager().addAttrib( handle2, "float_attrib" );
        m.attribManager().getAttrib( handle2 ).data().push_back( 1.f );
        m.attribManager().getAttrib( handle2 ).data().push_back( 2.f );
        m.attribManager().getAttrib( handle2 ).data().push_back( 3.f );
```

