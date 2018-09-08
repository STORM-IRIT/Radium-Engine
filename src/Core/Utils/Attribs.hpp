#ifndef RADIUMENGINE_ATTRIBS_HPP
#define RADIUMENGINE_ATTRIBS_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Index/IndexMap.hpp>
#include <Core/Log/Log.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

template <typename T>
class Attrib;

/// AttribBase is the base class for attributes of all type.
class AttribBase {
  public:
    virtual ~AttribBase() {}
    std::string getName() const { return m_name; }
    void setName( std::string name ) { m_name = name; }
    virtual void resize( size_t s ) = 0;

    virtual uint getSize() = 0;
    virtual int getStride() = 0;

    bool inline operator==( const AttribBase& rhs ) { return m_name == rhs.getName(); }

    template <typename T>
    inline Attrib<T>& cast() {
        return static_cast<Attrib<T>&>( *this );
    }

    template <typename T>
    inline const Attrib<T>& cast() const {
        return static_cast<const Attrib<T>&>( *this );
    }

    virtual bool isFloat() const = 0;
    virtual bool isVec2() const = 0;
    virtual bool isVec3() const = 0;
    virtual bool isVec4() const = 0;

  private:
    std::string m_name;
};

/// An Attribute is a vector of a given type.
template <typename T>
class Attrib : public AttribBase {
  public:
    using value_type = T;
    using Container = VectorArray<T>;

    /// resize the container (value_type must have a default ctor).
    void resize( size_t s ) override { m_data.resize( s ); }

    /// RW acces to container data
    inline Container& data() { return m_data; }

    /// R only acccess to container data
    inline const Container& data() const { return m_data; }

    virtual ~Attrib() { m_data.clear(); }
    uint getSize() override { return Container::Matrix::RowsAtCompileTime; }
    int getStride() override { return sizeof( typename Container::value_type ); }

    bool isFloat() const override { return std::is_same<float, T>::value; }
    bool isVec2() const override { return std::is_same<Core::Vector2, T>::value; }
    bool isVec3() const override { return std::is_same<Core::Vector3, T>::value; }
    bool isVec4() const override { return std::is_same<Core::Vector4, T>::value; }

  private:
    Container m_data;
};

template <typename T>
class AttribHandle {
  public:
    typedef T value_type;
    using Container = typename Attrib<T>::Container;

    /// There is no validity check against the corresponding mesh, but just a
    /// simple test to allow the manipuation of unitialized handles.
    constexpr bool isValid() const { return m_idx.isValid(); }

    /// compare two handle, there are the same if they both represent the same
    /// attrib (type and value).
    template <typename U>
    bool operator==( const AttribHandle<U>& lhs ) const {
        return std::is_same<T, U>::value && m_idx == lhs.m_idx;
    }

    Ra::Core::Index idx() { return m_idx; }

  private:
    Ra::Core::Index m_idx = Ra::Core::Index::Invalid();

    friend class AttribManager;
};

/*!
 * \brief The AttribManager provides attributes management by handles
 *
 * The AttribManager stores a container of AttribBase, which can
 * be accessed and deleted (#removeAttrib) using a AttribHandle. Handles
 * are created from an attribute name using #addAttrib, and retrieved using
 * #getAttribHandler.
 *
 * Example of typical use case:
 * \code
 * // somewhere: creation
 * AttribManager mng;
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
 * \warning There is no error check on the handles attribute type.
 *
 */
class AttribManager {
  public:
    using value_type = AttribBase*;
    using Container = std::vector<value_type>;

    AttribManager() {}

    /// Copy constructor and assignment operator are forbidden.
    AttribManager( const AttribManager& m ) = delete;
    AttribManager& operator=( const AttribManager& m ) = delete;

    AttribManager( AttribManager&& m ) :
        m_attribs( std::move( m.m_attribs ) ),
        m_attribsIndex( std::move( m.m_attribsIndex ) ) {}

    AttribManager& operator=( AttribManager&& m ) {
        m_attribs = std::move( m.m_attribs );
        m_attribsIndex = std::move( m.m_attribsIndex );
        return *this;
    }

    /// Base copy, does nothing.
    void copyAttributes( const AttribManager& m ) {}

    /// Copy the given attributes from \m.
    /// \note If some attrib already exist, it will be replaced but not de-allocated.
    /// \note Invalid handles are ignored.
    template <class T, class... Handle>
    void copyAttributes( const AttribManager& m, const AttribHandle<T>& attr, Handle... attribs ) {
        if ( attr.isValid() )
        {
            // get attrib to copy
            auto a = m.getAttrib( attr );
            // add new attrib
            auto h = addAttrib<T>( a.getName() );
            getAttrib( h ).data() = a.data();
            // deal with other attribs
        }
        copyAttributes( m, attribs... );
    }

    /// Copy all attributes from \m.
    void copyAllAttributes( const AttribManager& m ) {
        for ( const auto& attr : m.m_attribs )
        {
            if ( attr == nullptr )
                continue;
            if ( attr->isFloat() )
            {
                auto h = addAttrib<float>( attr->getName() );
                getAttrib( h ).data() = static_cast<Attrib<float>*>( attr )->data();
            } else if ( attr->isVec2() )
            {
                auto h = addAttrib<Vector2>( attr->getName() );
                getAttrib( h ).data() = static_cast<Attrib<Vector2>*>( attr )->data();
            } else if ( attr->isVec3() )
            {
                auto h = addAttrib<Vector3>( attr->getName() );
                getAttrib( h ).data() = static_cast<Attrib<Vector3>*>( attr )->data();
            } else if ( attr->isVec4() )
            {
                auto h = addAttrib<Vector4>( attr->getName() );
                getAttrib( h ).data() = static_cast<Attrib<Vector4>*>( attr )->data();
            } else
                LOG( logWARNING )
                    << "Warning, mesh attribute " << attr->getName()
                    << " type is not supported (only float, vec2, vec3 nor vec4 are supported)";
        }
    }

    /// clear all attribs, invalidate handles and shallow copies.
    void clear() {
        for ( auto attr : m_attribs )
        {
            delete attr;
        }
        m_attribs.clear();
        m_attribsIndex.clear();
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
     * \note The complexity for accessing an attribute handle is O(log(n)).
     */
    template <typename T>
    inline AttribHandle<T> getAttribHandle( const std::string& name ) const {
        auto c = m_attribsIndex.find( name );
        AttribHandle<T> handle;
        if ( c != m_attribsIndex.end() )
        {
            handle.m_idx = c->second;
        }
        return handle;
    }

    /// Get attribute by handle.
    /// \note The complexity for accessing an attribute is O(1).
    template <typename T>
    inline Attrib<T>& getAttrib( AttribHandle<T> h ) {
        CORE_ASSERT( h.isValid(), "Trying to access attribute from an invalid handle." );
        return *static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ) );
    }

    /// Get attribute by handle (const).
    /// \note The complexity for accessing an attribute is O(1).
    template <typename T>
    inline const Attrib<T>& getAttrib( AttribHandle<T> h ) const {
        CORE_ASSERT( h.isValid(), "Trying to access attribute from an invalid handle." );
        return *static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ) );
    }

    /// Add attribute by name.
    /// \warning If an attribute with the same name already exists,
    ///          just returns a AttribHandle to it.
    /// \note The complexity for adding a new attribute is O(n).
    template <typename T>
    AttribHandle<T> addAttrib( const std::string& name ) {
        AttribHandle<T> h;

        // does the attrib already exist?
        auto it = std::find_if( m_attribs.begin(), m_attribs.end(), [&name]( const auto& attr ) {
            return ( attr != nullptr && attr->getName() == name );
        } );
        if ( it != m_attribs.end() )
        {
            LOG( logWARNING ) << "Attribute " << name << " already exists.";
            h.m_idx = std::distance( m_attribs.begin(), it );
            return h;
        }

        Attrib<T>* attrib = new Attrib<T>;
        attrib->setName( name );
        // look for a free slot
        it = std::find_if( m_attribs.begin(), m_attribs.end(),
                           []( const auto& attr ) { return attr == nullptr; } );
        if ( it != m_attribs.end() )
        {
            *it = attrib;
            h.m_idx = std::distance( m_attribs.begin(), it );
        } else
        {
            m_attribs.push_back( attrib );
            h.m_idx = m_attribs.size() - 1;
        }
        m_attribsIndex[name] = h.m_idx;

        return h;
    }

    /// Remove attribute by handle, invalidate the handles to this attribute.
    /// \note: If a new attribute is added, old invalidated handles may lead to
    ///        the new attribute.
    /// \note The complexity for removing an attribute is O(log(n)).
    void removeAttrib( const std::string& name ) {
        auto c = m_attribsIndex.find( name );
        if ( c != m_attribsIndex.end() )
        {
            Ra::Core::Index idx = c->second;
            delete m_attribs[idx];
            m_attribs[idx] = nullptr;
            m_attribsIndex.erase( c );
        }
    }

    /// Remove attribute by handle, invalidate the handles to this attribute.
    /// \note: If a new attribute is added, old invalidated handles may lead to
    ///        the new attribute.
    /// \note The complexity for removing an attribute is O(log(n)).
    template <typename T>
    void removeAttrib( AttribHandle<T> h ) {
        const auto& att = getAttrib( h ); // check the attribute exists
        removeAttrib( att.getName() );
    }

  private:
    /// Access to the list of attributes.
    /// \note The complexity for acessing the list of attributes is O(n).
    Container attribs() const {
        Container c;
        c.reserve( m_attribs.size() );
        for ( const auto& attr : m_attribs )
        {
            if ( attr != nullptr )
            {
                c.push_back( attr );
            }
        }
        c.shrink_to_fit();
        return c;
    }

    // Attrib list
    Container m_attribs;

    // Map between the attrib's name and its index
    std::map<std::string, Ra::Core::Index> m_attribsIndex;

    // Ease wrapper
    friend class TopologicalMesh;
};

} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ATTRIBS_HPP
