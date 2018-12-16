#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/Attribs.hpp>

namespace Ra {
namespace Core {

/// Simple Mesh structure that handles indexed polygonal mesh with vertex
/// attributes.
/// Attributes are unique per vertex, so that same position with different
/// normals are two vertices.
/// Points and Normals, defining the mesh geometry, are always present.
/// They can be accessed through vertices() and normals().
/// Other attribs could be added with addAttrib() and accesssed with getAttrib().
/// \note Attribute names "in_position" and "in_normal" are reserved.
/// \see MeshUtils for geometric functions operating on a mesh.
class TriangleMesh {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using PointAttribHandle = Utils::AttribHandle<Vector3>;
    using NormalAttribHandle = Utils::AttribHandle<Vector3>;
    using FloatAttribHandle = Utils::AttribHandle<float>;
    using Vec2AttribHandle = Utils::AttribHandle<Vector2>;
    using Vec3AttribHandle = Utils::AttribHandle<Vector3>;
    using Vec4AttribHandle = Utils::AttribHandle<Vector4>;
    using Face = VectorNui;

    /// Create an empty mesh.
    inline TriangleMesh() { initDefaultAttribs(); }

    /// Copy constructor, copy all the mesh data (faces, geometry, attributes).
    /// \note Handles on \p other are not valid for *this.
    inline TriangleMesh( const TriangleMesh& other );

    /// Move constructor, copy all the mesh data (faces, geometry, attributes).
    /// \note Handles on \p other are also valid for *this.
    inline TriangleMesh( TriangleMesh&& other );

    /// Assignment operator, copy all the mesh data (faces, geometry, attributes).
    /// \warning Handles on \p other are not valid for *this.
    inline TriangleMesh& operator=( const TriangleMesh& other );

    /// Move assignment, copy all the mesh data (faces, geometry, attributes).
    /// \note Handles on \p other are also valid for *this.
    inline TriangleMesh& operator=( TriangleMesh&& other );

    /// Appends another mesh to this one, but only if they have the same attributes.
    /// Return True if the mesh has been successfully appended.
    /// \warning There is no error check on the handles attribute type.
    inline bool append( const TriangleMesh& other );

    /// Erases all data, making the mesh empty.
    inline void clear();

    /// Access the vertices positions.
    inline PointAttribHandle::Container& vertices() {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }

    /// Access the vertices positions.
    inline const PointAttribHandle::Container& vertices() const {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }

    /// Access the vertices normals.
    inline NormalAttribHandle::Container& normals() {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    /// Access the vertices normals.
    inline const NormalAttribHandle::Container& normals() const {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    /// Return the Handle to the attribute with the given name if it exists, an
    /// invalid handle otherwise.
    /// \see AttribManager::getAttribHandle() for more info.
    /// \note Attribute names "in_position" and "in_normal" are reserved and will
    ///       give an invalid handle.
    template <typename T>
    Utils::AttribHandle<T> getAttribHandle( const std::string& name ) const {
        if ( name.compare( "in_position" ) == 0 || name.compare( "in_normal" ) == 0 )
            return Utils::AttribHandle<T>();
        return m_vertexAttribs.findAttrib<T>( name );
    }

    /// Return true if \p h refers to an existing attribute in *this.
    template <typename T>
    bool isValid( const Utils::AttribHandle<T>& h ) const {
        return m_vertexAttribs.isValid( h );
    }

    /// Get attribute by handle.
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    Utils::Attrib<T>& getAttrib( const Utils::AttribHandle<T>& h ) {
        return m_vertexAttribs.getAttrib( h );
    }

    /// Get attribute by handle (const).
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    const Utils::Attrib<T>& getAttrib( const Utils::AttribHandle<T>& h ) const {
        return m_vertexAttribs.getAttrib( h );
    }

    /// Add attribute with the given name.
    /// \see AttribManager::addAttrib() for more info.
    /// \note If \p name if a reserved name, then no attribute is added and an
    ///       invalid Handle is returned.
    template <typename T>
    Utils::AttribHandle<T> addAttrib( const std::string& name ) {
        if ( name.compare( "in_position" ) == 0 || name.compare( "in_normal" ) == 0 )
            return Utils::AttribHandle<T>();
        return m_vertexAttribs.addAttrib<T>( name );
    }

    /// Remove attribute by handle.
    /// \see AttribManager::removeAttrib() for more info.
    template <typename T>
    void removeAttrib(Utils::AttribHandle<T>& h ) {
        m_vertexAttribs.removeAttrib( h );
    }

    /// Erases all attributes, leaving the mesh with faces and geometry only.
    inline void clearAttributes();

    /// Copy only the mesh faces and geometry.
    /// The needed attributes can be copied through copyAttributes().
    /// \warning Deletes all attributes of *this.
    inline void copyBaseGeometry( const TriangleMesh& other );

    /// Copy only the required attributes from \p input. Existing attributes are
    /// kept untouched, except if overwritten by attributes copied from \p other.
    /// \return True if the attributes have been sucessfully copied, false otherwise.
    /// \note *this and \p input must have the same number of vertices.
    /// \warning The original handles are not valid for the mesh copy.
    template <typename... Handles>
    bool copyAttributes( const TriangleMesh& input, Handles... attribs );

    /// Copy all the attributes from \p input. Existing attributes are
    /// kept untouched, except if overwritten by attributes copied from \p other.
    /// \return True if the attributes have been sucessfully copied, false otherwise.
    /// \note *this and \p input must have the same number of vertices.
    /// \warning The original handles are not valid for the mesh copy.
    inline bool copyAllAttributes( const TriangleMesh& input );

  public:
    /// The list of triangles.
    VectorArray<Triangle> m_triangles;

    /// A list of non-triangular polygons.
    // FIXME: not used.
    VectorArray<Face> m_faces;

  private:
    /// The attrib manager.
    Utils::AttribManager m_vertexAttribs;

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

    /// Append the data of \p attr to the attribute with the same name.
    /// \warning There is no check on the existence of *this's attribute.
    /// \warning There is no error check on the handles attribute type.
    template <typename T>
    void append_attrib( Utils::AttribBase* attr ) {
        auto h = m_vertexAttribs.findAttrib<T>( attr->getName() );
        auto& v0 = m_vertexAttribs.getAttrib( h ).data();
        const auto& v1 = attr->cast<T>().data();
        v0.insert( v0.end(), v1.cbegin(), v1.cend() );
    }

    // Ease wrapper
    friend class TopologicalMesh;
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/TriangleMesh.inl>

#endif // RADIUMENGINE_TRIANGLEMESH_HPP
