#ifndef RADIUMENGINE_ATTRIBS_HPP
#define RADIUMENGINE_ATTRIBS_HPP

#include <map>

#include <Core/Containers/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/Index.hpp>
#include <Core/Utils/Observable.hpp>
namespace Ra {
namespace Core {

namespace Geometry {
// need forward declarations for friend classes outside of Utils namespace
class TopologicalMesh;
class TriangleMesh;
} // namespace Geometry

namespace Utils {

template <typename T>
class Attrib;

/**
 * AttribBase is the base class for attributes of all type.
 */
class RA_CORE_API AttribBase : public ObservableVoid
{
  public:
    explicit AttribBase( const std::string& name ) : m_name{name} {}
    virtual ~AttribBase() { notify(); }
    AttribBase( const AttribBase& ) = delete;
    AttribBase& operator=( const AttribBase& ) = delete;
    /**
     * Return the attribute's name.
     */
    std::string getName() const { return m_name; }

    /**
     * Set the attribute's name.
     */
    void setName( const std::string& name ) { m_name = name; }

    /**
     * Resize the attribute's content.
     */
    virtual void resize( size_t s ) = 0;

    /**
     * Return the number of elements in the attribute content.
     */
    virtual size_t getSize() const = 0;

    ///\todo rename getNumberOfComponent ?
    virtual size_t getElementSize() const = 0;

    /// return the size in byte of the container
    virtual size_t getBufferSize() const = 0;

    /**
     * Return the stride, in bytes, from one attribute address to the next one.
     */
    virtual int getStride() const = 0;

    /**
     * Return true if *this and \p rhs have the same name.
     */
    bool inline operator==( const AttribBase& rhs ) { return m_name == rhs.getName(); }

    /**
     * Downcast from AttribBase to Attrib<T>.
     */
    template <typename T>
    inline Attrib<T>& cast() {
        return static_cast<Attrib<T>&>( *this );
    }

    /**
     * Downcast from AttribBase to Attrib<T>.
     */
    template <typename T>
    inline const Attrib<T>& cast() const {
        return static_cast<const Attrib<T>&>( *this );
    }

    /**
     * Return true if the attribute content is of float type, false otherwise.
     */
    virtual bool isFloat() const = 0;

    /**
     * Return true if the attribute content is of Vector2 type, false otherwise.
     */
    virtual bool isVec2() const = 0;

    /**
     * Return true if the attribute content is of Vector3 type, false otherwise.
     */
    virtual bool isVec3() const = 0;

    /**
     * Return true if the attribute content is of Vector4 type, false otherwise.
     */
    virtual bool isVec4() const = 0;

    virtual const void* dataPtr() const = 0;

    bool isLocked() const { return m_isLocked; }
    void lock( bool isLocked = true ) {
        CORE_ASSERT( isLocked != m_isLocked, "double (un)lock" );
        m_isLocked = isLocked;
        if ( !m_isLocked ) notify();
    }
    void unlock() { lock( false ); }

  private:
    /// The attribute's name.
    std::string m_name;

    /// Is data access locked by a user ?
    bool m_isLocked{false};
};

/**
 * An Attrib stores an element of type \p T for each entry.
 */
template <typename T>
class Attrib : public AttribBase
{
  public:
    using value_type = T;
    using Container  = VectorArray<T>;

    explicit Attrib( const std::string& name ) : AttribBase( name ) {}

    virtual ~Attrib() { m_data.clear(); }
    /// Resize the container (value_type must have a default ctor).
    void resize( size_t s ) override { m_data.resize( s ); }

    /// Read-write access to the attribute content.
    inline Container& getDataWithLock() {
        lock();
        return m_data;
    }

    const void* dataPtr() const override { return m_data.data(); }

    void setData( const Container& data ) {
        CORE_ASSERT( !isLocked(), "try to set onto locked data" );
        m_data = data;
        notify();
    }

    void setData( Container&& data ) {
        CORE_ASSERT( !isLocked(), "try to set onto locked data" );
        m_data = std::move( data );
        notify();
    }

    /// Read-only acccess to the attribute content.
    inline const Container& data() const { return m_data; }

    size_t getSize() const override { return m_data.size(); }

    size_t getElementSize() const override;
    /// \warning Does not work for dynamic and sparse Eigen matrices.
    int getStride() const override { return sizeof( value_type ); }
    size_t getBufferSize() const override { return m_data.size() * sizeof( value_type ); }

    bool isFloat() const override { return std::is_same<float, T>::value; }
    bool isVec2() const override { return std::is_same<Eigen::Matrix<Scalar, 2, 1>, T>::value; }
    bool isVec3() const override { return std::is_same<Eigen::Matrix<Scalar, 3, 1>, T>::value; }
    bool isVec4() const override { return std::is_same<Eigen::Matrix<Scalar, 4, 1>, T>::value; }

    /// check if attrib is a given type, as in attr.isType<MyMatrix>()
    template <typename U>
    bool isType() {
        return std::is_same<U, T>::value;
    }

  private:
    Container m_data;
};

// fully specialization defined in .cpp
template <>
RA_CORE_API size_t Attrib<float>::getElementSize() const;
// template specialization defined in header.
template <typename T>
size_t Attrib<T>::getElementSize() const {
    return Attrib<T>::Container::Vector::RowsAtCompileTime;
}

template <typename T>
class AttribHandle
{
  public:
    using value_type = T;
    using AttribType = Attrib<T>;
    using Container  = typename Attrib<T>::Container;
    /// compare two handle, there are the same if they both represent the same
    /// attrib (type and value).
    template <typename U>
    bool operator==( const AttribHandle<U>& lhs ) const {
        return std::is_same<T, U>::value && m_idx == lhs.m_idx;
    }

    Index idx() const { return m_idx; }

    std::string attribName() const { return m_name; }

  private:
    Index m_idx        = Index::Invalid();
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
class RA_CORE_API AttribManager : public Observable<const std::string&>
{
  public:
    using value_type         = AttribBase;
    using pointer_type       = value_type*;
    using smart_pointer_type = std::unique_ptr<value_type>;
    using Container          = std::vector<smart_pointer_type>;

    AttribManager() {}

    /// Copy constructor and assignment operator are forbidden.
    AttribManager( const AttribManager& m ) = delete;
    AttribManager& operator=( const AttribManager& m ) = delete;

    AttribManager( AttribManager&& m ) :
        m_attribs( std::move( m.m_attribs ) ),
        m_attribsIndex( std::move( m.m_attribsIndex ) ),
        m_numAttribs( std::move( m.m_numAttribs ) ) {}

    AttribManager& operator=( AttribManager&& m ) {
        m_attribs      = std::move( m.m_attribs );
        m_attribsIndex = std::move( m.m_attribsIndex );
        m_numAttribs   = std::move( m.m_numAttribs );
        return *this;
    }

    ~AttribManager() { clear(); }

    /// Base copy, does nothing.
    void copyAttributes( const AttribManager& m ) { m_numAttribs = m.m_numAttribs; }

    /// Copy the given attributes from m.
    /// \note If some attrib already exists, it will be replaced.
    /// \note Invalid handles are ignored.
    template <class T, class... Handle>
    void copyAttributes( const AttribManager& m, const AttribHandle<T>& attr, Handle... attribs ) {
        if ( m.isValid( attr ) )
        {
            // get attrib to copy
            auto& a = m.getAttrib( attr );
            // add new attrib
            auto h = addAttrib<T>( a.getName() );
            // copy attrib data
            getAttrib( h ).setData( a.data() );
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
        auto itr = m_attribsIndex.find( h.attribName() );
        return h.m_idx != Index::Invalid() && itr != m_attribsIndex.end() && itr->second == h.m_idx;
    }

    /*!
     * \brief contains Check if an attribute with the given name exists.
     * \param name Name of the attribute.
     * \warning There is no error check on the attribute type.
     * \note The complexity for checking an attribute handle is O(log(n)).
     */
    inline bool contains( const std::string& name ) const {
        return m_attribsIndex.find( name ) != m_attribsIndex.end();
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
            handle.m_idx  = c->second;
            handle.m_name = c->first;
        }
        return handle;
    }

    /// Get attribute by handle.
    /// \note The complexity for accessing an attribute is O(1).
    /// \warning There is no check on the handle validity.
    template <typename T>
    inline Attrib<T>& getAttrib( const AttribHandle<T>& h ) {
        return *static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ).get() );
    }

    template <typename T>
    inline Attrib<T>* getAttribPtr( const AttribHandle<T>& h ) {
        return static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ).get() );
    }

    template <typename T>
    inline void setAttrib( const AttribHandle<T>& h,
                           const typename AttribHandle<T>::Container& data ) {
        static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ).get() )->setData( data );
    }

    template <typename T>
    inline void setAttrib( const AttribHandle<T>& h, typename AttribHandle<T>::Container&& data ) {
        static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ).get() )->setData( data );
    }

    /// Get attribute by handle (const).
    /// \note The complexity for accessing an attribute is O(1).
    /// \warning There is no check on the handle validity.
    template <typename T>
    inline const Attrib<T>& getAttrib( const AttribHandle<T>& h ) const {
        return *static_cast<Attrib<T>*>( m_attribs.at( h.m_idx ).get() );
    }

    AttribBase* getAttribBase( const std::string& name ) {
        auto c = m_attribsIndex.find( name );
        if ( c != m_attribsIndex.end() ) return m_attribs[c->second].get();

        return nullptr;
    }

    AttribBase* getAttribBase( const Index& idx ) {

        if ( idx.isValid() ) return m_attribs[idx].get();

        return nullptr;
    }

    /// Add attribute by name.
    /// \note If an attribute with the same name already exists,
    ///       just returns a AttribHandle to it.
    /// \note The complexity for adding a new attribute is O(n).
    template <typename T>
    AttribHandle<T> addAttrib( const std::string& name ) {
        // does the attrib already exist?
        AttribHandle<T> h = findAttrib<T>( name );
        if ( isValid( h ) ) return h;

        // create the attrib
        smart_pointer_type attrib = std::make_unique<Attrib<T>>( name );

        // look for a free slot
        auto it = std::find_if(
            m_attribs.begin(), m_attribs.end(), []( const auto& attr ) { return !attr; } );
        if ( it != m_attribs.end() )
        {
            it->swap( attrib );
            h.m_idx = std::distance( m_attribs.begin(), it );
        }
        else
        {
            m_attribs.push_back( std::move( attrib ) );
            h.m_idx = m_attribs.size() - 1;
        }
        m_attribsIndex[name] = h.m_idx;
        h.m_name             = name;
        ++m_numAttribs;

        notify( name );
        return h;
    }

    /// Remove attribute by handle, invalidates the handle.
    /// \warning If a new attribute is added, old invalidated handles may lead to
    ///          the new attribute.
    /// \note The complexity for removing an attribute is O(log(n)).
    template <typename T>
    void removeAttrib( AttribHandle<T>& h ) {
        auto c = m_attribsIndex.find( h.m_name );
        if ( c != m_attribsIndex.end() )
        {
            Index idx = c->second;
            m_attribs[idx].reset( nullptr );
            m_attribsIndex.erase( c );
        }
        h.m_idx   = Index::Invalid(); // invalidate whatever!
        auto name = h.m_name;
        h.m_name  = ""; // invalidate whatever!
        --m_numAttribs;
        notify( name );
    }

    /// Return true if *this and \p other have the same attributes, same amount
    /// and same names.
    /// \warning There is no check on the attribute type nor data.
    bool hasSameAttribs( const AttribManager& other );

    /// Perform \p fun on each attribute.
    // This is needed by the user to avoid caring about removed attributes (nullptr)
    // \todo reimplement as range for

    template <typename F>
    void for_each_attrib( const F& func ) const {
        for ( const auto& attr : m_attribs )
            if ( attr != nullptr ) func( attr.get() );
    }

    /// Perform \p fun on each attribute.
    // This is needed by the user to avoid caring about removed attributes (nullptr)
    // \todo keep non const version private
    template <typename F>
    void for_each_attrib( const F& func ) {
        for ( auto& attr : m_attribs )
            if ( attr != nullptr ) func( attr.get() );
    }

    int getNumAttribs() const { return m_numAttribs; }

  private:
    /// Attrib list, better using attribs() to go through.
    Container m_attribs;

    // Map between the attrib's name and its index, used to speedup finding the
    // handle index from the attribute name.
    std::map<std::string, Index> m_attribsIndex;

    // Ease wrapper
    friend class ::Ra::Core::Geometry::TopologicalMesh;
    friend class ::Ra::Core::Geometry::TriangleMesh;

    /// Count number of valid attribs
    int m_numAttribs{0};
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#endif // RADIUMENGINE_ATTRIBS_HPP
