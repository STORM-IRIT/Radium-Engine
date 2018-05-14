#ifndef RADIUMENGINE_TRIANGLEMESH_HPP
#define RADIUMENGINE_TRIANGLEMESH_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Math/LinearAlgebra.hpp>
#include <Core/Mesh/MeshTypes.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

template <typename T>
class VertexAttrib;

/// VertexAttribBase is the base class for VertexAttrib.
/// A VertexAttrib is data linked to Vertices of a mesh
/// It is also used for rendering, and
class VertexAttribBase {
  public:
    /// attrib name is used to automatic location binding when using shaders.
    virtual ~VertexAttribBase() {}
    std::string getName() const { return m_name; }
    void setName( std::string name ) { m_name = name; }
    virtual void resize( size_t s ) = 0;

    virtual uint getSize() = 0;
    virtual int getStride() = 0;

    bool inline operator==( const VertexAttribBase& rhs ) { return m_name == rhs.getName(); }

    template <typename T>
    inline VertexAttrib<T>& cast() {
        return static_cast<VertexAttrib<T>&>( *this );
    }

    template <typename T>
    inline const VertexAttrib<T>& cast() const {
        return static_cast<const VertexAttrib<T>&>( *this );
    }

  private:
    std::string m_name;
};

template <typename T>
class VertexAttrib : public VertexAttribBase {
  public:
    using value_type = T;
    using Container = VectorArray<T>;

    /// resize the container (value_type must have a default ctor).
    void resize( size_t s ) override { m_data.resize( s ); }

    /// RW acces to container data
    inline Container& data() { return m_data; }

    /// R only acccess to container data
    inline const Container& data() const { return m_data; }

    virtual ~VertexAttrib() { m_data.clear(); }
    uint getSize() override { return Container::Matrix::RowsAtCompileTime; }
    int getStride() override { return sizeof( typename Container::value_type ); }

  private:
    Container m_data;
};

template <typename T>
class VertexAttribHandle {
  public:
    typedef T value_type;
    using Container = typename VertexAttrib<T>::Container;

    /// There is no validity check against the corresponding mesh, but just a
    /// simple test to allow the manipuation of unitialized handles.
    constexpr bool isValid() const { return m_idx != -1; }

  private:
    int m_idx = -1;

    friend class VertexAttribManager;
};

/*!
 * \brief The VertexAttribManager provides attributes management by handles
 *
 * The VertexAttribManager stores a container of VertexAttribBase, which can
 * be accessed and deleted (#removeAttrib) using a VertexAttribHandle. Handles
 * are created from an attribute name using #addAttrib, and retrieved using
 * #getAttribHandler.
 *
 * Example of typical use case:
 * \code
 * // somewhere: creation
 * VertexAttribManager mng;
 * auto inputfattrib = mng.addAttrib<float>("MyAttrib");
 *
 * ...
 *
 * // somewhere else: access
 * auto iattribhandler = mng.getAttribHandler<float>("MyAttrib"); //  iattribhandler == inputfattrib
 * if (iattribhandler.isValid()) {
 *    auto &attrib = mng.getAttrib( iattribhandler );
 * }
 * \endcode
 *
 * \warning There is no error check on the handles attribute type
 *
 */
class VertexAttribManager {
  public:
    using value_type = VertexAttribBase*;
    using Container = std::vector<value_type>;

    const Container& attribs() const { return m_attribs; }
    /// clear all attribs, invalidate handles !
    void clear() {
        for ( auto a : m_attribs )
        {
            delete a;
        }
        m_attribs.clear();
    }

    /*!
     * \brief getAttrib Grab an attribute handler by name
     * \param name Name of the attribute
     * \return Attribute handler
     * \code
     * VertexAttribManager mng;
     * auto inputfattrib = mng.addAttrib<float>("MyAttrib");
     *
     * auto iattribhandler = mng.getAttribHandler<float>("MyAttrib"); //  iattribhandler ==
     * inputfattrib if (iattribhandler.isValid()) { // true auto &attrib = mng.getAttrib(
     * iattribhandler );
     * }
     * auto& iattribhandler = mng.getAttribHandler<float>("InvalidAttrib"); // invalid
     * if (iattribhandler.isValid()) { // false
     *    ...
     * }
     * \endcode
     * \warning There is no error check on the attribute type
     */
    template <typename T>
    inline VertexAttribHandle<T> getAttribHandler( const std::string& name ) const {
        auto c = m_attribsIndex.find( name );
        VertexAttribHandle<T> handle;
        if ( c != m_attribsIndex.end() )
        {
            handle.m_idx = c->second;
        }
        return handle;
    }

    /// Get attribute by handle
    template <typename T>
    inline VertexAttrib<T>& getAttrib( VertexAttribHandle<T> h ) {
        return *static_cast<VertexAttrib<T>*>( m_attribs[h.m_idx] );
    }

    /// Get attribute by handle (const)
    template <typename T>
    inline const VertexAttrib<T>& getAttrib( VertexAttribHandle<T> h ) const {
        return *static_cast<VertexAttrib<T>*>( m_attribs[h.m_idx] );
    }

    /// Add attribute by name
    template <typename T>
    VertexAttribHandle<T> addAttrib( const std::string& name ) {
        VertexAttribHandle<T> h;
        VertexAttrib<T>* attrib = new VertexAttrib<T>;
        attrib->setName( name );
        m_attribs.push_back( attrib );
        h.m_idx = m_attribs.size() - 1;
        m_attribsIndex[name] = h.m_idx;
        return h;
    }

    /// Remove attribute by name, invalidate all the handles
    void removeAttrib( const std::string& name ) {
        auto c = m_attribsIndex.find( name );
        if ( c != m_attribsIndex.end() )
        {
            int idx = c->second;
            delete m_attribs[idx];
            m_attribs.erase( m_attribs.begin() + idx );
            m_attribsIndex.erase( c );

            // reindex attribs with index superior to removed index
            for ( auto& d : m_attribsIndex )
            {
                if ( d.second > idx )
                {
                    --d.second;
                }
            }
        }
    }

    /// Remove attribute by handle, invalidate all the handles
    template <typename T>
    void removeAttrib( VertexAttribHandle<T> h ) {
        const auto& att = getAttrib( h ); // check the attribute exists
        removeAttrib( att.getName() );
    }

  private:
    std::map<std::string, int> m_attribsIndex;
    Container m_attribs;
};

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
struct TriangleMesh {
  public:
    EIGEN_MAKE_ALIGNED_OPERATOR_NEW

    using PointAttribHandle = VertexAttribHandle<Vector3>;
    using NormalAttribHandle = VertexAttribHandle<Vector3>;
    using Vec3AttribHandle = VertexAttribHandle<Vector3>;
    using Vec4AttribHandle = VertexAttribHandle<Vector4>;
    using Face = VectorNui;

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

    PointAttribHandle::Container& vertices() {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }
    NormalAttribHandle::Container& normals() {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    const PointAttribHandle::Container& vertices() const {
        return m_vertexAttribs.getAttrib( m_verticesHandle ).data();
    }
    const NormalAttribHandle::Container& normals() const {
        return m_vertexAttribs.getAttrib( m_normalsHandle ).data();
    }

    const VertexAttribManager& attribManager() const { return m_vertexAttribs; }
    VertexAttribManager& attribManager() { return m_vertexAttribs; }

  private:
    VertexAttribManager m_vertexAttribs;
    PointAttribHandle m_verticesHandle;
    NormalAttribHandle m_normalsHandle;

    inline void initDefaultAttribs() {
        m_verticesHandle =
            m_vertexAttribs.addAttrib<PointAttribHandle::value_type>( "in_position" );
        m_normalsHandle = m_vertexAttribs.addAttrib<NormalAttribHandle::value_type>( "in_normal" );
    }
};

} // namespace Core
} // namespace Ra

#include <Core/Mesh/TriangleMesh.inl>

#endif // RADIUMENGINE_TRIANGLEMESH_HPP
