#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/Attribs.hpp>

namespace Ra
{
namespace Core
{

/// Simple Mesh structure that handles indexed triangle mesh with vertex
/// attributes.
/// Attributes are unique per vertex, so that same position with different
/// normals are two vertices.
/// The VertexAttribManager allows to dynammicaly add VertexAttrib per Vertex
/// See MeshUtils for geometric functions operating on a mesh.
/// Points and Normals are always present, accessible with
/// points() and normals()
/// Other attribs could be added with attribManager().addAttrib() and
/// accesssed with attribManager().getAttrib()
class TriangleMesh
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using PointAttribHandle  = AttribHandle<Vector3>;
    using NormalAttribHandle = AttribHandle<Vector3>;
    using FloatAttribHandle  = AttribHandle<float>;
    using Vec2AttribHandle   = AttribHandle<Vector2>;
    using Vec3AttribHandle   = AttribHandle<Vector3>;
    using Vec4AttribHandle   = AttribHandle<Vector4>;
    using Face               = VectorNui;

    /// Create an empty mesh.
    inline TriangleMesh() { initDefaultAttribs(); }
    /// Copy constructor and assignment operator
    TriangleMesh( const TriangleMesh& ) = default;
    TriangleMesh& operator=( const TriangleMesh& ) = default;

    /// Erases all data, making the mesh empty.
    inline void clear();

    /// Appends another mesh to this one.
    /// \todo handle attrib here as well !
    inline void append( const TriangleMesh& other );

    VectorArray<Triangle> m_triangles;
    VectorArray<Face> m_faces;

    PointAttribHandle::Container& vertices()
    {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }
    NormalAttribHandle::Container& normals()
    {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    const PointAttribHandle::Container& vertices() const
    {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }
    const NormalAttribHandle::Container& normals() const
    {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    const AttribManager& attribManager() const { return m_vertexAttribs; }
    AttribManager& attribManager() { return m_vertexAttribs; }

  private:
    AttribManager m_vertexAttribs;
    PointAttribHandle m_verticesHandle;
    NormalAttribHandle m_normalsHandle;

    inline void initDefaultAttribs()
    {
        m_verticesHandle =
            m_vertexAttribs.addAttrib<PointAttribHandle::value_type>( "in_position" );
        m_normalsHandle = m_vertexAttribs.addAttrib<NormalAttribHandle::value_type>( "in_normal" );
    }
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/TriangleMesh.inl>

#endif // RADIUMENGINE_TRIANGLEMESH_HPP
