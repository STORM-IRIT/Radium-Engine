#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Geometry/AbstractGeometry.hpp>
#include <Core/RaCore.hpp>
#include <Core/Types.hpp>
#include <Core/Utils/Attribs.hpp>
#include <Core/Utils/Color.hpp>
#include <Core/Utils/Observable.hpp>

namespace Ra {
namespace Core {
namespace Geometry {

/// Attributes are unique per vertex, so that same position with different
/// normals are two vertices.
/// Points and Normals, defining the mesh geometry, are always present.
/// They can be accessed through vertices() and normals().
/// Other attribs could be added with addAttrib() and accesssed with getAttrib().
/// \note Attribute names "in_position" "in_normal" are reserved and pre-allocated.
class RA_CORE_API AttribArrayGeometry : public AbstractGeometry
{
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using Point  = Vector3;
    using Normal = Vector3;

    using PointAttribHandle  = Utils::AttribHandle<Point>;
    using NormalAttribHandle = Utils::AttribHandle<Normal>;
    using FloatAttribHandle  = Utils::AttribHandle<float>;
    using Vec2AttribHandle   = Utils::AttribHandle<Vector2>;
    using Vec3AttribHandle   = Utils::AttribHandle<Vector3>;
    using Vec4AttribHandle   = Utils::AttribHandle<Vector4>;

    /// Create an empty mesh.
    inline AttribArrayGeometry() : AbstractGeometry() { initDefaultAttribs(); }

    /// Copy constructor, copy all the mesh data (faces, geometry, attributes).
    /// \note Handles on \p other are not valid for *this.
    inline explicit AttribArrayGeometry( const AttribArrayGeometry& other );

    /// Move constructor, copy all the mesh data (faces, geometry, attributes).
    /// \note Handles on \p other are also valid for *this.
    inline explicit AttribArrayGeometry( AttribArrayGeometry&& other );

    /// Assignment operator, copy all the mesh data (faces, geometry, attributes).
    /// \warning Handles on \p other are not valid for *this.
    inline AttribArrayGeometry& operator=( const AttribArrayGeometry& other );

    /// Move assignment, copy all the mesh data (faces, geometry, attributes).
    /// \note Handles on \p other are also valid for *this.
    inline AttribArrayGeometry& operator=( AttribArrayGeometry&& other );

    ~AttribArrayGeometry() = default;

    /// Appends another mesh to this one, but only if they have the same attributes.
    /// Return True if the mesh has been successfully appended.
    /// \warning There is no error check on the handles attribute type.
    bool append( const AttribArrayGeometry& other );

    /// Erases all data, making the mesh empty.
    void clear() override;

    /// Set vertices
    inline void setVertices( PointAttribHandle::Container&& vertices );
    inline void setVertices( const PointAttribHandle::Container& vertices );

    /// Access the vertices positions.
    inline const PointAttribHandle::Container& vertices() const;

    /// Set normals
    inline void setNormals( PointAttribHandle::Container&& normals );
    inline void setNormals( const PointAttribHandle::Container& normals );
    /// Access the vertices normals.
    inline const NormalAttribHandle::Container& normals() const;

    /// Return the Handle to the attribute with the given name if it exists, an
    /// invalid handle otherwise.
    /// \see AttribManager::getAttribHandle() for more info.
    template <typename T>
    inline Utils::AttribHandle<T> getAttribHandle( const std::string& name ) const;

    /// Return true if \p h refers to an existing attribute in *this.
    template <typename T>
    inline bool isValid( const Utils::AttribHandle<T>& h ) const;

    /// Get attribute by handle.
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    inline Utils::Attrib<T>& getAttrib( const Utils::AttribHandle<T>& h );

    template <typename T>
    inline Utils::Attrib<T>* getAttribPtr( const Utils::AttribHandle<T>& h );

    /// Get attribute by handle (const).
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    inline const Utils::Attrib<T>& getAttrib( const Utils::AttribHandle<T>& h ) const;

    inline Utils::AttribBase* getAttribBase( const std::string& name );

    /// Check if an attribute exists with the given name.
    /// \see AttribManager::contains for more info.
    inline bool hasAttrib( const std::string& name );

    /// Add attribute with the given name.
    /// \see AttribManager::addAttrib() for more info.
    template <typename T>
    inline Utils::AttribHandle<T> addAttrib( const std::string& name );

    template <typename T>
    inline Utils::AttribHandle<T> addAttrib( const std::string& name,
                                             const typename Core::VectorArray<T>& data ) {
        auto handle = addAttrib<T>( name );
        getAttrib( handle ).setData( data );
        return handle;
    }

    template <typename T>
    inline Utils::AttribHandle<T> addAttrib( const std::string& name,
                                             const typename Utils::Attrib<T>::Container&& data ) {
        auto handle = addAttrib<T>( name );
        getAttrib( handle ).setData( std::move( data ) );
        return handle;
    }

    /// Remove attribute by handle.
    /// \see AttribManager::removeAttrib() for more info.
    template <typename T>
    inline void removeAttrib( Utils::AttribHandle<T>& h );

    /// Erases all attributes, leaving the mesh with faces and geometry only.
    inline void clearAttributes();

    /// Copy only the mesh faces and geometry.
    /// The needed attributes can be copied through copyAttributes().
    /// \warning Deletes all attributes of *this.
    inline virtual void copyBaseGeometry( const AttribArrayGeometry& other );

    /// Copy only the required attributes from \p input. Existing attributes are
    /// kept untouched, except if overwritten by attributes copied from \p other.
    /// \return True if the attributes have been sucessfully copied, false otherwise.
    /// \note *this and \p input must have the same number of vertices.
    /// \warning The original handles are not valid for the mesh copy.
    template <typename... Handles>
    bool copyAttributes( const AttribArrayGeometry& input, Handles... attribs );

    /// Copy all the attributes from \p input. Existing attributes are
    /// kept untouched, except if overwritten by attributes copied from \p other.
    /// \return True if the attributes have been sucessfully copied, false otherwise.
    /// \note *this and \p input must have the same number of vertices.
    /// \warning The original handles are not valid for the mesh copy.
    inline bool copyAllAttributes( const AttribArrayGeometry& input );

    inline Aabb computeAabb() const override;

    /// Utility function colorzing the mesh with a given color. Add the color attribute if needed.
    void colorize( const Utils::Color& c );

    inline Utils::AttribManager& vertexAttribs();

    /// Access the vertices positions.
    inline PointAttribHandle::Container& verticesWithLock();
    /// Access the vertices positions.
    inline void verticesUnlock();

    /// Access the vertices positions.
    inline NormalAttribHandle::Container& normalsWithLock();

    /// Access the vertices positions.
    inline void normalsUnlock();

  private:
    /// Sets the default attribs.
    inline void initDefaultAttribs();
    /// Append the data of \p attr to the attribute with the same name.
    /// \warning There is no check on the existence of *this's attribute.
    /// \warning There is no error check on the handles attribute type.
    template <typename T>
    void append_attrib( Utils::AttribBase* attr );

    /// The attrib manager.
    Utils::AttribManager m_vertexAttribs;

    /// The handle for positions, making request faster.
    PointAttribHandle m_verticesHandle;

    /// The handle for normals, making request faster.
    NormalAttribHandle m_normalsHandle;

    // Ease wrapper
    friend class TopologicalMesh;
};

class RA_CORE_API PointCloud : public AttribArrayGeometry
{};

class RA_CORE_API LineStrip : public AttribArrayGeometry
{};

/// Simple Mesh structure that handles indexed polygonal mesh with vertex
/// attributes. Each face is indexed with typename T = IndexType.
/// T is assumed to be an Eigen Vector of unsigned ints
template <typename T>
class RA_CORE_API IndexedGeometry : public AttribArrayGeometry
{
  public:
    using IndexType          = T;
    using IndexContainerType = AlignedStdVector<IndexType>;

    inline IndexedGeometry() = default;
    inline explicit IndexedGeometry( const IndexedGeometry<IndexType>& other );
    inline explicit IndexedGeometry( IndexedGeometry<IndexType>&& other );
    inline IndexedGeometry<IndexType>& operator=( const IndexedGeometry<IndexType>& other );
    inline IndexedGeometry<IndexType>& operator=( IndexedGeometry<IndexType>&& other );
    inline void clear() override;
    inline void copy( const IndexedGeometry<IndexType>& other );

    /// Check that the mesh is well built, asserting when it is not.
    /// only compiles to something when in debug mode.
    inline void checkConsistency() const;
    inline bool append( const IndexedGeometry<IndexType>& other );

    ///\todo make it protected
    IndexContainerType m_indices;
};

class RA_CORE_API IndexedPointCloud : public IndexedGeometry<Vector1ui>
{};

class RA_CORE_API TriangleMesh : public IndexedGeometry<Vector3ui>
{};

class RA_CORE_API LineMesh : public IndexedGeometry<Vector2ui>
{};

} // namespace Geometry
} // namespace Core
} // namespace Ra

#include <Core/Geometry/TriangleMesh.inl>

#endif // RADIUMENGINE_TRIANGLEMESH_HPP
