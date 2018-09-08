#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/Attribs.hpp>

namespace Ra {
namespace Core {

/// Simple Mesh structure that handles indexed triangle mesh with vertex
/// attributes.
/// Attributes are unique per vertex, so that same position with different
/// normals are two vertices.
/// Points and Normals are always present, accessible with
/// points() and normals().
/// Other attribs could be added with addAttrib() and
/// accesssed with getAttrib().
/// \see MeshUtils for geometric functions operating on a mesh.
/// \note Attribute names "in_position" and "in_normal" are reserved.
class TriangleMesh {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using PointAttribHandle = AttribHandle<Vector3>;
    using NormalAttribHandle = AttribHandle<Vector3>;
    using FloatAttribHandle = AttribHandle<float>;
    using Vec2AttribHandle = AttribHandle<Vector2>;
    using Vec3AttribHandle = AttribHandle<Vector3>;
    using Vec4AttribHandle = AttribHandle<Vector4>;
    using Face = VectorNui;

    /// Create an empty mesh.
    inline TriangleMesh() { initDefaultAttribs(); }

    /// Copy constructor, copy only the mesh topology,
    /// the list of vertices and vertices normals.
    /// For attributes copy, use the copyAttributes() or copyAllAttributes() methods.
    inline TriangleMesh( const TriangleMesh& other );

    /// Move constructor, copy all the mesh data (topology, geometry, attributes).
    inline TriangleMesh( TriangleMesh&& other );

    /// Assignment operator copy only the mesh topology,
    /// the list of vertices and vertices normals.
    /// For attributes copy, use the copyAttributes() or copyAllAttributes() methods.
    inline TriangleMesh& operator=( const TriangleMesh& other );

    /// Move assignment, copy all the mesh data (topology, geometry, attributes).
    inline TriangleMesh& operator=( TriangleMesh&& other );

    /// Erases all data, making the mesh empty.
    inline void clear();

    /// Appends another mesh to this one.
    /// \todo handle attribs here as well !
    inline void append( const TriangleMesh& other );

    /// Access the vertices positions.
    PointAttribHandle::Container& vertices() {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }
    const PointAttribHandle::Container& vertices() const {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }

    /// Access the vertices normals.
    NormalAttribHandle::Container& normals() {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }
    const NormalAttribHandle::Container& normals() const {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    /// Return the Handle to the attribute with the given name.
    /// \see AttribManager::getAttribHandle() for more info.
    template <typename T>
    inline AttribHandle<T> getAttribHandle( const std::string& name ) const {
        return m_vertexAttribs.getAttribHandle<T>( name );
    }

    /// Get attribute by handle.
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    inline Attrib<T>& getAttrib( AttribHandle<T> h ) {
        return m_vertexAttribs.getAttrib( h );
    }

    /// Get attribute by handle (const).
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    inline const Attrib<T>& getAttrib( AttribHandle<T> h ) const {
        return m_vertexAttribs.getAttrib( h );
    }

    /// Add attribute with the given name.
    /// \see AttribManager::addAttrib() for more info.
    /// \note If \p name if a reserved name, then no attribute is added and an
    ///       invalid Handle is returned.
    template <typename T>
    AttribHandle<T> addAttrib( const std::string& name ) {
        if ( name.compare( "in_position" ) == 0 || name.compare( "in_normal" ) == 0 )
            return AttribHandle<T>();
        return m_vertexAttribs.addAttrib<T>( name );
    }

    /// Remove attribute by handle, invalidate the handles to this attribute.
    /// \see AttribManager::removeAttrib() for more info.
    template <typename T>
    void removeAttrib( AttribHandle<T> h ) {
        m_vertexAttribs.removeAttrib( h );
    }

    /// Copy only the required attributes (deep copy).
    /// The position and normal attributes are always copied, no need to provide their handles.
    /// \warning The original handles are not valid for the mesh copy.
    /// \see AttribManager::copyAttributes() for more info.
    template <typename... Handles>
    void copyAttributes( const TriangleMesh& input, Handles... attribs );

    /// Copy all the attributes (deep copy).
    /// \warning The original handles are also valid for the mesh copy.
    /// \see AttribManager::copyAllAttributes() for more info.
    inline void copyAllAttributes( const TriangleMesh& input );

  public:
    /// The list of triangles.
    VectorArray<Triangle> m_triangles;

    /// A list of non-triangular polygons.
    // FIXME: not used.
    VectorArray<Face> m_faces;

  private:
    /// The attrib manager.
    AttribManager m_vertexAttribs;

    /// The handle for positions, making request faster.
    PointAttribHandle m_verticesHandle;

    /// The handle for normals, making request faster.
    NormalAttribHandle m_normalsHandle;

    /// Sets the default attribs.
    inline void initDefaultAttribs() {
        m_verticesHandle =
            m_vertexAttribs.addAttrib<PointAttribHandle::value_type>( "in_position" );
        m_normalsHandle = m_vertexAttribs.addAttrib<NormalAttribHandle::value_type>( "in_normal" );
    }

    /// Access the attribute manager
    const AttribManager& attribManager() const { return m_vertexAttribs; }
    AttribManager& attribManager() { return m_vertexAttribs; }

    // Ease wrapper
    friend class TopologicalMesh;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/TriangleMesh.inl>

#endif // RADIUMENGINE_TRIANGLEMESH_HPP
