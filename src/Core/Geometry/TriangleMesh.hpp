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
/// Points and Normals, defining the geometry, are always present.
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

    /// Create an empty geometry.
    inline AttribArrayGeometry() : AbstractGeometry() { initDefaultAttribs(); }

    /// Copy constructor, copy all the data (geometry, attributes).
    /// \note Handles on \p other are not valid for *this.
    inline explicit AttribArrayGeometry( const AttribArrayGeometry& other );

    /// Move constructor, copy all the data (geometry, attributes).
    /// \note Handles on \p other are also valid for *this.
    inline explicit AttribArrayGeometry( AttribArrayGeometry&& other );

    /// Assignment operator, copy all the data (geometry, attributes).
    /// \warning Handles on \p other are not valid for *this.
    inline AttribArrayGeometry& operator=( const AttribArrayGeometry& other );

    /// Move assignment, copy all the data (geometry, attributes).
    /// \note Handles on \p other are also valid for *this.
    inline AttribArrayGeometry& operator=( AttribArrayGeometry&& other );

    ~AttribArrayGeometry() = default;

    /// Appends another AttribArrayGeometry to this one, but only if they have the same attributes.
    /// Return True if \p other has been successfully appended.
    /// \warning There is no error check on the handles attribute type.
    bool append( const AttribArrayGeometry& other );

    /// Erases all data, making the AttribArrayGeometry empty.
    void clear() override;

    /// Set vertices
    inline void setVertices( PointAttribHandle::Container&& vertices );
    /// Set vertices
    inline void setVertices( const PointAttribHandle::Container& vertices );

    /// Access the vertices positions.
    inline const PointAttribHandle::Container& vertices() const;

    /// Set normals
    inline void setNormals( PointAttribHandle::Container&& normals );
    /// Set normals
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

    /// Get attribute by handle.
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    inline Utils::Attrib<T>* getAttribPtr( const Utils::AttribHandle<T>& h );

    /// Get attribute by handle (const).
    /// \see AttribManager::getAttrib() for more info.
    template <typename T>
    inline const Utils::Attrib<T>& getAttrib( const Utils::AttribHandle<T>& h ) const;

    /// Get attribute by handle (const).
    /// \see AttribManager::getAttrib() for more info.
    inline Utils::AttribBase* getAttribBase( const std::string& name );

    /// Check if an attribute exists with the given name.
    /// \see AttribManager::contains for more info.
    inline bool hasAttrib( const std::string& name );

    ///@{
    /// Add attribute with the given name.
    /// \see AttribManager::addAttrib() for more info.
    /// \param name: attrib name, uniquely identify the attrib
    /// \return handle to set the attrib data with getAttrib(handle).setData()
    template <typename T>
    inline Utils::AttribHandle<T> addAttrib( const std::string& name );

    /// Add attribute with the given name.
    /// \see AttribManager::addAttrib() for more info.
    /// \param name: attrib name, uniquely identify the attrib
    /// \param data: an array defining the attrib data
    template <typename T>
    inline Utils::AttribHandle<T> addAttrib( const std::string& name,
                                             const typename Core::VectorArray<T>& data );

    /// Add attribute with the given name.
    /// \see AttribManager::addAttrib() for more info.
    /// \param name: attrib name, uniquely identify the attrib
    /// \param data: an array defining the attrib data, the data is moved to the attrib
    template <typename T>
    inline Utils::AttribHandle<T> addAttrib( const std::string& name,
                                             const typename Utils::Attrib<T>::Container&& data );
    ///@}

    /// Remove attribute by handle.
    /// \see AttribManager::removeAttrib() for more info.
    template <typename T>
    inline void removeAttrib( Utils::AttribHandle<T>& h );

    /// Erases all attributes, leaving the AttribArrayGeometry with geometry only.
    inline void clearAttributes();

    /// Copy only the geometry.
    /// The needed attributes can be copied through copyAttributes().
    /// \warning Deletes all attributes of *this.
    inline virtual void copyBaseGeometry( const AttribArrayGeometry& other );

    /// Copy only the required attributes from \p input. Existing attributes are
    /// kept untouched, except if overwritten by attributes copied from \p other.
    /// \return True if the attributes have been sucessfully copied, false otherwise.
    /// \note *this and \p input must have the same number of vertices.
    /// \warning The original handles are not valid for the AttribArrayGeometry copy.
    template <typename... Handles>
    bool copyAttributes( const AttribArrayGeometry& input, Handles... attribs );

    /// Copy all the attributes from \p input. Existing attributes are
    /// kept untouched, except if overwritten by attributes copied from \p other.
    /// \return True if the attributes have been sucessfully copied, false otherwise.
    /// \note *this and \p input must have the same number of vertices.
    /// \warning The original handles are not valid for the AttribArrayGeometry copy.
    inline bool copyAllAttributes( const AttribArrayGeometry& input );

    inline Aabb computeAabb() const override;

    /// Utility function colorzing the AttribArrayGeometry with a given color.
    /// \note Add the color attribute if needed.
    void colorize( const Utils::Color& c );

    /// Return the vertexAttribs manager. In case you want to have direct access
    /// to Utils::AttribManager functionality.
    inline Utils::AttribManager& vertexAttribs();

    /// Return the vertexAttribs manager. In case you want to have direct access
    /// to Utils::AttribManager functionality.
    inline const Utils::AttribManager& vertexAttribs() const;

    /// Read/write access to the vertices positions.
    /// \warning need to call verticesUnlock when job done.
    inline PointAttribHandle::Container& verticesWithLock();

    /// Release lock on vertices positions
    inline void verticesUnlock();

    /// Read/write access the vertices normals.
    /// \warning need to call normalsUnlock when job done.
    inline NormalAttribHandle::Container& normalsWithLock();

    /// Release lock on vertices normals
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
};

class RA_CORE_API PointCloud : public AttribArrayGeometry
{};

class RA_CORE_API LineStrip : public AttribArrayGeometry
{};

/// Simple Mesh structure that handles indexed polygonal mesh with vertex
/// attributes. Each face is indexed with typename T = IndexType.
/// T is assumed to be an Eigen Vector of unsigned ints
template <typename T>
class IndexedGeometry : public AttribArrayGeometry
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

    /// Copy only the geometry and the indices from \p other, but not the attributes.
    inline void copy( const IndexedGeometry<IndexType>& other );

    /// Check that the IndexedGeometry is well built, asserting when it is not.
    /// only compiles to something when in debug mode.
    inline void checkConsistency() const;

    /// Appends another IndexedGeometry to this one, but only if they have the same attributes.
    /// Return True if \p other has been successfully appended.
    /// \warning There is no error check on the handles attribute type.
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
