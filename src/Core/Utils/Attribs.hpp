#ifndef RADIUMENGINE_ATTRIBS_HPP
#define RADIUMENGINE_ATTRIBS_HPP

#include <Core/Containers/VectorArray.hpp>
#include <Core/Index/Index.hpp>
#include <Core/RaCore.hpp>

namespace Ra {
namespace Core {

// need forward declarations for friend classes outside of Utils namespace
class TopologicalMesh; 
class TriangleMesh; 

namespace Utils {

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
    bool isVec2() const override { return std::is_same<Vector2, T>::value; }
    bool isVec3() const override { return std::is_same<Vector3, T>::value; }
    bool isVec4() const override { return std::is_same<Vector4, T>::value; }

  private:
    Container m_data;
};

template <typename T>
class AttribHandle {
  public:
    typedef T value_type;
    using Container = typename Attrib<T>::Container;

    /// compare two handle, there are the same if they both represent the same
    /// attrib (type and value).
    template <typename U>
    bool operator==( const AttribHandle<U>& lhs ) const {
        return std::is_same<T, U>::value && m_idx == lhs.m_idx;
    }

    Index idx() const { return m_idx; }

    std::string attribName() const { return m_name; }

  private:
    Index m_idx = Index::Invalid();
    std::string m_name = "";

    friend class AttribManager;
};

/*!
 * \brief The AttribManager provides attributes management by handles.
 *
 * The AttribManager stores a container of AttribBase *, which can
 * be accessed (#getAttrib) and deleted (#removeAttrib) using a AttribHandle.
 * Handles are created from an attribute name using #addAttrib, and retrieved
 * using #findAttrib.
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
 * auto iattribhandler = mng.findAttrib<float>("MyAttrib"); //  iattribhandler == inputfattrib
 * if ( mng.isValid( iattribhandler ) ) { // true
 *     auto &attrib = mng.getAttrib( iattribhandler );
 *     ...
 * }
 * auto& iattribhandler = mng.findAttrib<float>("InvalidAttrib"); // invalid
 * if ( mng.isValid( iattribhandler ) ) { // false
 *    ...
 * }
 * \endcode
 *
 * \warning There is no error check on the handles attribute type.
 *
 */
class RA_CORE_API AttribManager {
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

    ~AttribManager() { clear(); }

    /// Base copy, does nothing.
    void copyAttributes( const AttribManager& m ) {}

    /// Copy the given attributes from m.
    /// \note If some attrib already exists, it will be replaced.
    /// \note Invalid handles are ignored.
    template <class T, class... Handle>
    void copyAttributes( const AttribManager& m, const AttribHandle<T>& attr, Handle... attribs ) {
        if ( m.isValid( attr ) )
        {
            // get attrib to copy
            auto a = m.getAttrib( attr );
            // add new attrib
            auto h = addAttrib<T>( a.getName() );
            // copy attrib data
            getAttrib( h ).data() = a.data();
        }
        // deal with other attribs
        copyAttributes( m, attribs... );
    }

    /// Copy all attributes from m.
    /// \note If some attrib already exists, it will be replaced.
    void copyAllAttributes( const AttribManager& m );

    /// clear all attribs, invalidate handles.
    void clear();

    /// Return true if \p h correspond to an existing attribute in *this.
    template <typename T>
    bool isValid( const AttribHandle<T>& h ) const {
        return h.m_idx != Index::Invalid() && m_attribsIndex.at( h.attribName() ) == h.m_idx;
    }

    /*!
     * \brief findAttrib Grab an attribute handler by name.
     * \param name Name of the attribute.
     * \return Attribute handler if found, an invalid handler otherwise.
     * \warning There is no error check on the attribute type.
     * \note The complexity for accessing an attribute handle is O(log(n)).
     */
    template <typename T>
    inline AttribHandle<T> findAttrib( const std::string& name ) const {
        auto c = m_attribsIndex.find( name );
        AttribHandle<T> handle;
        if ( c != m_attribsIndex.end() )
        {
            handle.m_idx = c->second;
            handle.m_name = c->first;
        }
        return handle;
    }

    /// Get attribute by handle.
    /// \note The complexity for accessing an attribute is O(1).
    /// \warning There is no check on the handle validity.
    template <typename T>
    inline Attrib<T>& getAttrib( const AttribHandle<T>& h ) {
        return *static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ) );
    }

    /// Get attribute by handle (const).
    /// \note The complexity for accessing an attribute is O(1).
    /// \warning There is no check on the handle validity.
    template <typename T>
    inline const Attrib<T>& getAttrib( const AttribHandle<T>& h ) const {
        return *static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ) );
    }

    /// Add attribute by name.
    /// \note If an attribute with the same name already exists,
    ///       just returns a AttribHandle to it.
    /// \note The complexity for adding a new attribute is O(n).
    template <typename T>
    AttribHandle<T> addAttrib( const std::string& name ) {
        // does the attrib already exist?
        AttribHandle<T> h = findAttrib<T>( name );
        if ( isValid( h ) )
            return h;

        // create the attrib
        Attrib<T>* attrib = new Attrib<T>;
        attrib->setName( name );

        // look for a free slot
        auto it = std::find_if( m_attribs.begin(), m_attribs.end(),
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
        h.m_name = name;

        return h;
    }

    /// Remove attribute by handle, invalidates the handle.
    /// \warning If a new attribute is added, old invalidated handles may lead to
    ///          the new attribute.
    /// \note The complexity for removing an attribute is O(log(n)).
    template <typename T>
    void removeAttrib( AttribHandle<T>& h ) {
        const auto& att = getAttrib( h );
        auto c = m_attribsIndex.find( att.getName() );
        if ( c != m_attribsIndex.end() )
        {
            Index idx = c->second;
            delete m_attribs[idx];
            m_attribs[idx] = nullptr;
            m_attribsIndex.erase( c );
        }
        h.m_idx = Index::Invalid(); // invalidate whatever!
        h.m_name = "";              // invalidate whatever!
    }

    /// Return true if *this and \p other have the same attributes, same amount
    /// and same names.
    /// \warning There is no check on the attribute type nor data.
    bool hasSameAttribs( const AttribManager& other );

  private:
    /// Perform \p fun on each attribute.
    // This is needed by the user to avoid caring about removed attributes (nullptr)
    template <typename F>
    void for_each_attrib( const F& func ) const {
        for ( const auto& attr : m_attribs )
            if ( attr != nullptr )
                func( attr );
    }

    /// Perform \p fun on each attribute.
    // This is needed by the user to avoid caring about removed attributes (nullptr)
    template <typename F>
    void for_each_attrib( const F& func ) {
        for ( auto& attr : m_attribs )
            if ( attr != nullptr )
                func( attr );
    }

    /// Attrib list, better using attribs() to go through.
    Container m_attribs;

    // Map between the attrib's name and its index, used to speedup finding the
    // handle index from the attribute name.
    std::map<std::string, Index> m_attribsIndex;

    // Ease wrapper
    friend class ::Ra::Core::TopologicalMesh;
    friend class ::Ra::Core::TriangleMesh;
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ATTRIBS_HPP
