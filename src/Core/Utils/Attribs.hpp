#pragma once
#include <map>

#include <Core/Containers/VectorArray.hpp>
#include <Core/RaCore.hpp>
#include <Core/Utils/ContainerIntrospectionInterface.hpp>
#include <Core/Utils/Index.hpp>
#include <Core/Utils/Observable.hpp>

namespace Ra {
namespace Core {

namespace Geometry {
// need forward declarations for friend classes outside of Utils namespace
class TopologicalMesh;
} // namespace Geometry

namespace Utils {

template <typename T>
class Attrib;

/**
 * AttribBase is the base class for attributes of all type.
 */
class RA_CORE_API AttribBase : public ObservableVoid, public ContainerIntrospectionInterface
{
  public:
    inline explicit AttribBase( const std::string& name );
    virtual ~AttribBase();
    AttribBase( const AttribBase& )            = delete;
    AttribBase& operator=( const AttribBase& ) = delete;

    /// Return the attribute's name.
    inline std::string getName() const;

    ///    Set the attribute's name.
    inline void setName( const std::string& name );

    /// Resize the attribute's array.
    virtual void resize( size_t s ) = 0;

    /// Return true if *this and \p rhs have the same name.
    bool inline operator==( const AttribBase& rhs );

    /// Downcast from AttribBase to Attrib<T>.
    template <typename T>
    inline Attrib<T>& cast();

    /// Downcast from AttribBase to Attrib<T>.
    template <typename T>
    inline const Attrib<T>& cast() const;

    /// Return true if the attribute content is of Scalar type, false otherwise.
    virtual bool isFloat() const = 0;

    /// Return true if the attribute content is of Vector2 type, false otherwise.
    virtual bool isVector2() const = 0;

    /// Return true if the attribute content is of Vector3 type, false otherwise.
    virtual bool isVector3() const = 0;

    /// Return true if the attribute content is of Vector4 type, false otherwise.
    virtual bool isVector4() const = 0;

    /// Return true if data is locked, i.e. has been locked for write access with
    /// getDataWithlock() (defined in subclass Attrib). Double lock is prohebited, so when finished,
    /// call unlock();
    bool inline isLocked() const;

    /// Unlock data so another one can gain write access.
    void inline unlock();

    virtual std::unique_ptr<AttribBase> clone() = 0;

  protected:
    void inline lock( bool isLocked = true );

  private:
    /// The attribute's name.
    std::string m_name;

    /// Is data access locked by a user ?
    bool m_isLocked { false };
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

    explicit Attrib( const std::string& name );
    virtual ~Attrib();

    /// Resize the container (value_type must have a default ctor).
    void resize( size_t s ) override;

    /// Read-write access to the attribute content.
    /// lock the content, when done call unlock()
    inline Container& getDataWithLock();

    /// @{
    /// ContainerIntrosectionInterface implementation
    size_t getSize() const override;
    size_t getNumberOfComponents() const override;
    int getStride() const override;
    size_t getBufferSize() const override;
    const void* dataPtr() const override;
    /// @}

    ///@{
    /// setAttribData, attrib mustn't be locked (it's asserted).
    void setData( const Container& data );
    void setData( Container&& data );
    ///@}

    /// Read-only acccess to the attribute content.
    inline const Container& data() const;
    bool isFloat() const override;
    bool isVector2() const override;
    bool isVector3() const override;
    bool isVector4() const override;

    /// check if attrib is a given type, as in attr.isType<MyMatrix>()
    template <typename U>
    bool isType();

    std::unique_ptr<AttribBase> clone() override {
        auto ptr    = std::make_unique<Attrib<T>>( getName() );
        ptr->m_data = m_data;
        return ptr;
    }

  private:
    Container m_data;
};

/// An attrib handle basically store an Index and a name.
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
        return std::is_same<T, U>::value && m_idx == lhs.idx();
    }

    /// return the index of the attrib.
    Index idx() const { return m_idx; }

    /// return the name of the attrib.
    /// attrib name are unique in a given attribManager.
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
 * auto inputfattrib = mng.addAttrib<Scalar>("MyAttrib");
 *
 * ...
 *
 * // somewhere else: access
 * auto iattribhandler = mng.findAttrib<Scalar>("MyAttrib"); //  iattribhandler == inputfattrib
 * if ( mng.isValid( iattribhandler ) ) { // true
 *     auto &attrib = mng.getAttrib( iattribhandler );
 *     ...
 * }
 * auto& iattribhandler = mng.findAttrib<Scalar>("InvalidAttrib"); // invalid
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

    inline AttribManager();

    /// Copy constructor and assignment operator are forbidden.
    AttribManager( const AttribManager& m )            = delete;
    AttribManager& operator=( const AttribManager& m ) = delete;

    inline AttribManager( AttribManager&& m );

    inline AttribManager& operator=( AttribManager&& m );
    ~AttribManager() override;

    /// Base copy, does nothing.
    inline void copyAttributes( const AttribManager& m );

    /// Copy the given attributes from \p m.
    /// \note If some attrib already exists, it will be replaced.
    /// \note Invalid handles are ignored.
    template <class T, class... Handle>
    void copyAttributes( const AttribManager& m, const AttribHandle<T>& attr, Handle... attribs );

    /// Copy all attributes from \p m.
    /// \note If some attrib already exists, it will be replaced.
    /// \todo allow to copy all attributes, even those with non-standard type
    void copyAllAttributes( const AttribManager& m );

    /// clear all attribs, invalidate handles.
    void clear();

    /// Return true if \p h correspond to an existing attribute in *this.
    template <typename T>
    bool isValid( const AttribHandle<T>& h ) const;

    /*!
     * \brief contains Check if an attribute with the given \p name exists.
     * \param name Name of the attribute.
     * \warning There is no error check on the attribute type.
     * \complexity \f$ O(\log(n)) \f$
     */
    inline bool contains( const std::string& name ) const;

    /*!
     * \brief findAttrib Grab an attribute handler by \p name.
     * \param name Name of the attribute.
     * \return Attribute handler if found, an invalid handler otherwise.
     * \warning There is no error check on the attribute type.
     * \complexity \f$ O(\log(n)) \f$
     */
    template <typename T>
    inline AttribHandle<T> findAttrib( const std::string& name ) const;

    /*!
     * \brief Get the locked data container from the attrib handle
     * \tparam T The type of the attribute data
     * \param h the attribute handle
     * \return the attribute's data container
     */
    template <typename T>
    typename Attrib<T>::Container& getDataWithLock( const AttribHandle<T>& h );

    /*!
     * \brief Get read access to the data container from the attrib handle
     * \tparam T The type of the attribute data
     * \param h the attribute handle
     * \return the attribute's data container
     */
    template <typename T>
    const typename Attrib<T>::Container& getData( const AttribHandle<T>& h );

    /*!
     * \brief Unlock the handle data
     * \tparam T
     * \param h
     * \return
     */
    template <typename T>
    void unlock( const AttribHandle<T>& h );

    ///@{
    /// Get attribute by handle.
    /// \complexity \f$ O(1) \f$
    /// \warning There is no check on the handle validity. Attrib is statically cast to T without
    /// other checks.
    ///
    template <typename T>
    inline Attrib<T>& getAttrib( const AttribHandle<T>& h );
    template <typename T>
    inline const Attrib<T>& getAttrib( const AttribHandle<T>& h ) const;
    template <typename T>
    inline Attrib<T>* getAttribPtr( const AttribHandle<T>& h );
    template <typename T>
    inline const Attrib<T>* getAttribPtr( const AttribHandle<T>& h ) const;
    ///@}

    ///@{
    /// Get attribute by name.
    /// First search name to index correpondance \complexity \f$ O(getNumAttribs()) \f$
    /// \warning There is no check on the name validity. Attrib is statically cast to T without
    /// other checks.
    template <typename T>
    inline Attrib<T>& getAttrib( const std::string& name );
    template <typename T>
    inline const Attrib<T>& getAttrib( const std::string& name ) const;
    ///@}

    ///@{
    /// Return a AttribBase ptr to the attrib identified by name.
    /// to give access to AttribBase method, regardless of the type of element
    /// stored in the attrib.
    inline AttribBase* getAttribBase( const std::string& name );
    inline const AttribBase* getAttribBase( const std::string& name ) const;
    inline AttribBase* getAttribBase( const Index& idx );
    inline const AttribBase* getAttribBase( const Index& idx ) const;
    ///@}

    ///@{
    /// Set the data of the attrib h.
    /// \warning \p h has to be a valid attrib handle, this is not checked.
    template <typename T>
    inline void setAttrib( const AttribHandle<T>& h,
                           const typename AttribHandle<T>::Container& data );

    /// \see #setAttrib
    template <typename T>
    inline void setAttrib( const AttribHandle<T>& h, typename AttribHandle<T>::Container&& data );
    ///@}

    /// Add attribute by name.
    /// notify(name) is called to trigger the observers
    /// \note If an attribute with the same name already exists,
    ///       just returns a AttribHandle to it.
    /// \complexity \f$ O(n) \f$
    template <typename T>
    AttribHandle<T> addAttrib( const std::string& name );

    /// Remove attribute by handle, invalidates the handle.
    /// notify(name) is called to trigger the observers
    /// \warning If a new attribute is added, old invalidated handles may lead to
    ///          the new attribute.
    /// \complexity \f$ O(\log(n)) \f$
    template <typename T>
    void removeAttrib( AttribHandle<T>& h );
    /// Return true if *this and \p other have the same attributes, same amount
    /// and same names.
    /// \warning There is no check on the attribute type nor data.
    bool hasSameAttribs( const AttribManager& other );

    ///@{
    /// Perform \p fun on each attribute.
    // This is needed by the user to avoid caring about removed attributes (nullptr)
    // \todo reimplement as range for
    template <typename F>
    void for_each_attrib( const F& func ) const;

    // \todo keep non const version private
    template <typename F>
    void for_each_attrib( const F& func );
    ///@}

    /// Return the number of attributes
    inline int getNumAttribs() const;

    /**
     * \brief Scope lock state management for attributes.
     *
     * Unlock all attribs locked after the creation of the ScopedLockState object when it
     * gets out of scope.
     * \code{.cpp}
     * {
     * auto g = new AttribArrayGeometry();
     *
     * // get write access. The attrib must be explicitly unlocked after usage
     * auto& attrib1 = g->getAttrib<Ra::Core::Vector3>( "attrib1" ).getDataWithLock();
     * // ... write to attribOne
     *
     * // enable auto-unlock for all following write access requests
     * auto unlocker = g->vertexAttribs().getScopedLockState();
     *
     * // get write access to several attribs
     * auto& attrib2 = g->getAttrib<Ra::Core::Vector3>( "attrib2" ).getDataWithLock();
     * // ... write to attrib2
     * auto& attrib3 = g->getAttrib<Ra::Core::Vector3>( "attrib3" ).getDataWithLock();
     * // ... write to attrib3
     *
     * // attrib1 is still locked as it was locked outside of the unlocker scope.
     * // Must be explicitly unlocked.
     * attrib1.unlock();
     *
     * // attrib2 and attrib3 are automatically unlocked when the unlocker's destructor
     * // is called (i.e. gets out of scope)
     * }
     * \endcode
     */
    class ScopedLockState
    {
      public:
        /**
         * \param a the AttribManager on which locking will be supervised
         * \brief Constructor, save lock state of all attribs from attribManager
         */
        explicit ScopedLockState( AttribManager* a ) {
            a->for_each_attrib( [this]( const auto& attr ) {
                return ( v.push_back( std::make_pair( attr, attr->isLocked() ) ) );
            } );
        }
        /**
         * \brief Destructor, unlock all attribs whose have been locked after the
         * initialization of the Unlocker.
         */
        ~ScopedLockState() {
            for ( auto& p : v ) {
                if ( !p.second && p.first->isLocked() ) { p.first->unlock(); }
            }
        }

      private:
        std::vector<std::pair<AttribBase*, bool>> v;
    };

    /// Returns a scope unlocker for managed attribs
    ScopedLockState getScopedLockState() { return ScopedLockState { this }; }

  private:
    /// Attrib list, better using attribs() to go through.
    Container m_attribs;

    /// Map between the attrib's name and its index, used to speedup finding the handle index from
    /// the attribute name.
    std::map<std::string, Index> m_attribsIndex;

    // Ease wrapper
    friend class ::Ra::Core::Geometry::TopologicalMesh;

    /// Count number of valid attribs
    int m_numAttribs { 0 };
};

} // namespace Utils
} // namespace Core
} // namespace Ra

#include <Core/Utils/Attribs.inl>
