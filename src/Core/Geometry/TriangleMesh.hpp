#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Attribs.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/**
 * Simple Mesh structure that handles indexed polygonal mesh with vertex
 * attributes.
 * Attributes are unique per vertex, so that same position with different
 * normals are two vertices.
 * Points and Normals, defining the mesh geometry, are always present.
 * They can be accessed through vertices() and normals().
 * Other attribs could be added with addAttrib() and accesssed with getAttrib().
 * \note Attribute names "in_position" and "in_normal" are reserved.
 */
class RA_CORE_API TriangleMesh {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using PointAttribHandle = Utils::AttribHandle<Vector3>;
    using NormalAttribHandle = Utils::AttribHandle<Vector3>;
    using FloatAttribHandle = Utils::AttribHandle<float>;
    using Vec2AttribHandle = Utils::AttribHandle<Vector2>;
    using Vec3AttribHandle = Utils::AttribHandle<Vector3>;
    using Vec4AttribHandle = Utils::AttribHandle<Vector4>;
    using Face = VectorNui;

    /**
     * Create an empty mesh.
     */
    inline TriangleMesh() { initDefaultAttribs(); }

    /**
     * Copy constructor, copies all the mesh data (faces, geometry, attributes).
     * \note AttribHandles on \p other are not valid for *this.
     */
    inline TriangleMesh( const TriangleMesh& other );

    /**
     * Move constructor, copy all the mesh data (faces, geometry, attributes).
     * \note AttribHandles on \p other are also valid for *this.
     */
    inline TriangleMesh( TriangleMesh&& other );

    /**
     * Assignment operator, copy all the mesh data (faces, geometry, attributes).
     * \warning AttribHandles on \p other are not valid for *this.
     */
    inline TriangleMesh& operator=( const TriangleMesh& other );

    /**
     * Move assignment, copy all the mesh data (faces, geometry, attributes).
     * \note AttribHandles on \p other are also valid for *this.
     */
    inline TriangleMesh& operator=( TriangleMesh&& other );

    /**
     * Appends another mesh to this one, but only if they have the same attributes.
     * Return True if the mesh has been successfully appended.
     * \warning There is no error check on the attibutes type, only names matter.
     */
    bool append( const TriangleMesh& other );

    /**
     * Erases all data, making the mesh empty.
     */
    inline void clear();

    /**
     * Access the vertices positions.
     */
    inline PointAttribHandle::Container& vertices() {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }

    /**
     * Access the vertices positions.
     */
    inline const PointAttribHandle::Container& vertices() const {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }

    /**
     * Access the vertices normals.
     */
    inline NormalAttribHandle::Container& normals() {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    /**
     * Access the vertices normals.
     */
    inline const NormalAttribHandle::Container& normals() const {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    /**
     * Return the AttribHandle to the attribute with the given name if it exists,
     * an invalid AttribHandle otherwise.
     * \see AttribManager::getAttribHandle() for more info.
     * \note Attribute names "in_position" and "in_normal" are reserved and will
     *       give an invalid AttribHandle.
     */
    template <typename T>
    Utils::AttribHandle<T> getAttribHandle( const std::string& name ) const {
        if ( name.compare( "in_position" ) == 0 || name.compare( "in_normal" ) == 0 )
            return Utils::AttribHandle<T>();
        return m_vertexAttribs.findAttrib<T>( name );
    }

    /**
     * Return true if \p h refers to an existing attribute in *this.
     */
    template <typename T>
    bool isValid( const Utils::AttribHandle<T>& h ) const {
        return m_vertexAttribs.isValid( h );
    }

    /**
     * Get attribute by AttribHandle.
     * \see AttribManager::getAttrib() for more info.
     */
    template <typename T>
    Utils::Attrib<T>& getAttrib( const Utils::AttribHandle<T>& h ) {
        return m_vertexAttribs.getAttrib( h );
    }

    /**
     * Get attribute by AttribHandle (const).
     * \see AttribManager::getAttrib() for more info.
     */
    template <typename T>
    const Utils::Attrib<T>& getAttrib( const Utils::AttribHandle<T>& h ) const {
        return m_vertexAttribs.getAttrib( h );
    }

    /**
     * Add attribute with the given name.
     * \see AttribManager::addAttrib() for more info.
     * \note If \p name if a reserved name, then no attribute is added and an
     *       invalid AttribHandle is returned.
     */
    template <typename T>
    Utils::AttribHandle<T> addAttrib( const std::string& name ) {
        if ( name.compare( "in_position" ) == 0 || name.compare( "in_normal" ) == 0 )
            return Utils::AttribHandle<T>();
        return m_vertexAttribs.addAttrib<T>( name );
    }

    /**
     * Remove attribute by handle.
     * \see AttribManager::removeAttrib() for more info.
     */
    template <typename T>
    void removeAttrib( Utils::AttribHandle<T>& h ) {
        m_vertexAttribs.removeAttrib( h );
    }

    /**
     * Erases all attributes, leaving the mesh with faces and geometry only.
     */
    void clearAttributes();

    /**
     * Copy only the mesh faces and geometry.
     * The needed attributes can be copied through copyAttributes().
     * \warning Deletes all attributes of *this.
     */
    inline void copyBaseGeometry( const TriangleMesh& other );

    /**
     * Copy only the required attributes from \p input. Existing attributes are
     * kept untouched, except if overwritten by attributes copied from \p other.
     * \return True if the attributes have been sucessfully copied, false otherwise.
     * \note *this and \p input must have the same number of vertices.
     * \warning The original AttribHandles are not valid for the mesh copy.
     */
    template <typename... Handles>
    bool copyAttributes( const TriangleMesh& input, Handles... attribs );

    /**
     * Copy all the attributes from \p input. Existing attributes are
     * kept untouched, except if overwritten by attributes copied from \p other.
     * \return True if the attributes have been sucessfully copied, false otherwise.
     * \note *this and \p input must have the same number of vertices.
     * \warning The original AttribHandles are not valid for the mesh copy.
     */
    inline bool copyAllAttributes( const TriangleMesh& input );

    /**
     * Return the Aabb for the mesh.
     */
    inline Aabb computeAabb() const;

    /**
     * Check that the mesh is well built, asserting when it is not.
     * \note Only compiles to something when in debug mode.
     */
    void checkConsistency() const;

    /**
     * Results of a raycast vs a mesh.
     */
    struct RayCastResult {
        int m_hitTriangle = -1;   ///< Index of the hit triangle.
        int m_nearestVertex = -1; ///< Index of the closest mesh vertex.
        int m_edgeVertex0 = -1;   ///< Index of the first vertex of the closest edge.
        int m_edgeVertex1 = -1;   ///< Index of the second vertex of the closest edge.
        Scalar m_t = -1;          ///< Linear parameter of the intersection along the Ray.
    };

    /**
     * Return the result of casting the given ray on the TriangleMesh.
     */
    RayCastResult castRay( const Core::Ray& ray ) const;

  public:
    /// The list of triangles.
    VectorArray<Vector3ui> m_triangles;

    /// The list of non-triangular polygons.
    VectorArray<Face> m_faces;

  private:
    /**
     * Sets the default attribs.
     */
    inline void initDefaultAttribs() {
        m_verticesHandle =
            m_vertexAttribs.addAttrib<PointAttribHandle::value_type>( "in_position" );
        m_normalsHandle = m_vertexAttribs.addAttrib<NormalAttribHandle::value_type>( "in_normal" );
    }

    /**
     * Append the data of \p attr to the attribute with the same name.
     * \warning There is no check on the existence of *this's attribute.
     * \warning There is no error check on the AttribHandles attribute type.
     */
    template <typename T>
    void append_attrib( Utils::AttribBase* attr ) {
        auto h = m_vertexAttribs.findAttrib<T>( attr->getName() );
        auto& v0 = m_vertexAttribs.getAttrib( h ).data();
        const auto& v1 = attr->cast<T>().data();
        v0.insert( v0.end(), v1.cbegin(), v1.cend() );
    }

  private:
    /// The attrib manager.
    Utils::AttribManager m_vertexAttribs;

    /// The AttribHandle for positions, making request faster.
    PointAttribHandle m_verticesHandle;

    /// The AttribHandle for normals, making request faster.
    NormalAttribHandle m_normalsHandle;

    // Ease wrapper.
    friend class TopologicalMesh;
};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/TriangleMesh.inl>

#endif // RADIUMENGINE_TRIANGLEMESH_HPP
