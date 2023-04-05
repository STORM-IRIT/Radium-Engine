#pragma once

#include <Core/RaCore.hpp>

#include <algorithm>
#include <assert.h>
#include <deque>

#include <Core/Utils/Index.hpp>

namespace Ra {
namespace Core {
namespace Utils {

/*!
 * The class IndexMap define a map where a object is coupled with a index.
 * The index is unique, it is assigned to a object when it's inserted and is kept until the object
 * is removed. After a removal, the index becomes free again. A object will be given the first free
 * index available. If no free indices are available, the object will not be inserted and the
 * IndexMap is considered full.
 */
template <typename T>
class IndexMap
{
  public:
    // ===============================================================================
    // TYPEDEF
    // ===============================================================================
    using Container      = typename std::deque<T>;     /// Where the objects are stored
    using IndexContainer = typename std::deque<Index>; /// Where the indices are stored

    using ConstIndexIterator =
        typename IndexContainer::const_iterator; /// Const iterator to the list of indices of the
                                                 /// IndexMap.
    using Iterator =
        typename Container::iterator; /// Iterator to the list of objects of the IndexMap.
    using ConstIterator = typename Container::const_iterator; /// Const iterator to the list of
                                                              /// objects of the IndexMap.

    // ===============================================================================
    // CONSTRUCTOR
    // ===============================================================================
    inline IndexMap();                         /// Default constructor.
    inline IndexMap( const IndexMap& id_map ); /// Copy constructor.

    // ===============================================================================
    // DESTRUCTOR
    // ===============================================================================
    inline ~IndexMap(); /// Destructor.

    // ===============================================================================
    // INSERT
    // ===============================================================================
    /// Insert a object in the IndexMap. Return an invalid index if the object is not inserted.
    inline Index insert( const T& obj );

    /// Construct an object in place in the IndexMap. Return an invalid index if the object is not
    /// inserted.
    template <typename... Args>
    inline Index emplace( const Args&&... args );

    // ===============================================================================
    // REMOVE
    // ===============================================================================
    /// Remove the object with the given index. Return false if the operation failed.
    inline bool remove( const Index& idx );

    // ===============================================================================
    // ACCESS
    // ===============================================================================
    /// Return a read-only ref to object with the given index.  Crashes if index does not exist.
    inline const T& at( const Index& idx ) const;

    /// Return a reference to the object with the given index. Crash if index does not exist
    inline T& access( const Index& idx );

    // ===============================================================================
    // SIZE
    // ===============================================================================
    inline size_t size() const; /// Return the size of the IndexMap ( number of object contained ).
    inline void clear();        /// Clear the IndexMap.

    // ===============================================================================
    // QUERY
    // ===============================================================================
    inline bool empty() const; /// Return true if the IndexMap is empty.
    inline bool full() const;  /// Return true if the IndexMap cannot contain more objects.
    inline bool contains( const Index& idx )
        const; /// Return true if the IndexMap contains a object with the given index.
    inline Index index( const uint i )
        const; /// Return the i-th index. Return an invalid index if i is out of bound.

    // ===============================================================================
    // OPERATOR
    // ===============================================================================
    inline T& operator[]( const Index& idx ); /// Return a reference to the object with given index.
    inline const T& operator[](
        const Index& idx ) const; /// Return a const reference to the object with given index.

    // ===============================================================================
    // INDEX ITERATOR
    // ===============================================================================
    inline ConstIndexIterator
    cbegin_index() const; /// Return a const iterator to the first index in the IndexMap.
    inline ConstIndexIterator
    cend_index() const; /// Return a const iterator to the end of the indices of the IndexMap.

    // ===============================================================================
    // DATA ITERATOR
    // ===============================================================================
    inline Iterator begin(); /// Return a iterator to the first object in the IndexMap.
    inline Iterator end();   /// Return a iterator to the end of the object list in the IndexMap.
    inline ConstIterator begin() const; /// Return a iterator to the first object in the IndexMap.
    inline ConstIterator
    end() const; /// Return a iterator to the end of the object list in the IndexMap.
    inline ConstIterator
    cbegin() const; /// Return a const iterator to the first object in the IndexMap.
    inline ConstIterator
    cend() const; /// Return a const iterator to the end of the object list in the IndexMap.

  protected:
    // Member variables
    Container m_data;       /// Objects in the IndexMap
    IndexContainer m_index; /// Indices in the IndexMap

  private:
    // ===============================================================================
    // FREE LIST MANAGEMENT
    // ===============================================================================
    inline void push_free_index( const Index& idx ); /// Push a new free index in free list
    inline bool pop_free_index( Index& idx );        /// Pop a free index from the free list

    // ===============================================================================
    // HELPER FUNCTIONS
    // ===============================================================================
    // These function return an iterator to an object from an interator in an index.
    inline ConstIterator citfromIndex( const ConstIndexIterator& it )
        const; /// Returns a const iterator on an object from its intex iterator
    inline Iterator
    itfromIndex( const ConstIndexIterator&
                     it ); /// Returns a non-const iterator on an object from its intex iterator
    inline size_t idxfromIndex( const ConstIndexIterator& it )
        const; /// Return the index of the object in the map from its index iterator

  private:
    // Member variables
    IndexContainer m_free; /// List of available free indices.
};

// ===============================================================================
// CONSTRUCTOR
// ===============================================================================
template <typename T>
IndexMap<T>::IndexMap() : m_data(), m_index(), m_free( 1, Index( 0 ) ) {}

template <typename T>
IndexMap<T>::IndexMap( const IndexMap& id_map ) :
    m_data( id_map.m_data ), m_index( id_map.m_index ), m_free( id_map.m_free ) {}

// ===============================================================================
// DESTRUCTOR
// ===============================================================================
template <typename T>
IndexMap<T>::~IndexMap() {}

// ===============================================================================
// INSERT
// ===============================================================================
template <typename T>
inline Index IndexMap<T>::insert( const T& obj ) {
    Index idx;
    if ( pop_free_index( idx ) ) {
        typename std::deque<Index>::iterator it =
            std::lower_bound( m_index.begin(), m_index.end(), idx );
        if ( it == m_index.end() ) { m_data.insert( m_data.end(), obj ); }
        else { m_data.insert( citfromIndex( it ), obj ); }
        m_index.insert( it, idx );
    }
    return idx;
}

template <typename T>
template <typename... Args>
Index IndexMap<T>::emplace( const Args&&... args ) {
    Index idx;
    if ( pop_free_index( idx ) ) {
        typename std::deque<Index>::iterator it =
            std::lower_bound( m_index.begin(), m_index.end(), idx );
        if ( it == m_index.end() ) { m_data.emplace( m_data.end(), args... ); }
        else { m_data.emplace( citfromIndex( it ), args... ); }
        m_index.insert( it, idx );
    }
    return idx;
}

// ===============================================================================
// REMOVE
// ===============================================================================
template <typename T>
inline bool IndexMap<T>::remove( const Index& idx ) {
    typename std::deque<Index>::iterator it = std::find( m_index.begin(), m_index.end(), idx );
    if ( it == m_index.end() ) { return false; }
    m_data.erase( itfromIndex( it ) );
    m_index.erase( it );
    push_free_index( idx );
    return true;
}

// ===============================================================================
// ACCESS
// ===============================================================================
template <typename T>
inline const T& IndexMap<T>::at( const Index& idx ) const {
    typename std::deque<Index>::const_iterator it =
        std::find( m_index.begin(), m_index.end(), idx );
    CORE_ASSERT( it != m_index.end(), "Index not found" );
    return m_data.at( idxfromIndex( it ) );
}

template <typename T>
inline T& IndexMap<T>::access( const Index& idx ) {
    typename std::deque<Index>::iterator it = std::find( m_index.begin(), m_index.end(), idx );
    CORE_ASSERT( ( it != m_index.end() ), "Index not found" );
    return *itfromIndex( it );
}

// ===============================================================================
// SIZE
// ===============================================================================
template <typename T>
inline size_t IndexMap<T>::size() const {
    return m_data.size();
}

template <typename T>
inline void IndexMap<T>::clear() {
    m_index.clear();
    m_data.clear();
    m_free.clear();
    m_free.push_back( Index( 0 ) );
}

// ===============================================================================
// QUERY
// ===============================================================================
template <typename T>
inline bool IndexMap<T>::empty() const {
    return m_data.empty();
}

template <typename T>
inline bool IndexMap<T>::full() const {
    return m_free.empty();
}

template <typename T>
inline bool IndexMap<T>::contains( const Index& idx ) const {
    typename std::deque<Index>::const_iterator it =
        std::find( m_index.begin(), m_index.end(), idx );
    return !( it == m_index.end() );
}

template <typename T>
inline Index IndexMap<T>::index( const uint i ) const {
    if ( i >= m_index.size() ) { return Index::Invalid(); }
    return m_index.at( i );
}

// ===============================================================================
// OPERATOR
// ===============================================================================
template <typename T>
inline T& IndexMap<T>::operator[]( const Index& idx ) {
    return access( idx );
}

template <typename T>
inline const T& IndexMap<T>::operator[]( const Index& idx ) const {
    return at( idx );
}

// ===============================================================================
// INDEX ITERATOR
// ===============================================================================
template <typename T>
inline typename IndexMap<T>::ConstIndexIterator IndexMap<T>::cbegin_index() const {
    return m_index.cbegin();
}

template <typename T>
inline typename IndexMap<T>::ConstIndexIterator IndexMap<T>::cend_index() const {
    return m_index.cend();
}

// ===============================================================================
// DATA ITERATOR
// ===============================================================================
template <typename T>
inline typename IndexMap<T>::Iterator IndexMap<T>::begin() {
    return m_data.begin();
}

template <typename T>
inline typename IndexMap<T>::Iterator IndexMap<T>::end() {
    return m_data.end();
}

template <typename T>
inline typename IndexMap<T>::ConstIterator IndexMap<T>::begin() const {
    return m_data.begin();
}

template <typename T>
inline typename IndexMap<T>::ConstIterator IndexMap<T>::end() const {
    return m_data.end();
}

template <typename T>
inline typename IndexMap<T>::ConstIterator IndexMap<T>::cbegin() const {
    return m_data.cbegin();
}

template <typename T>
inline typename IndexMap<T>::ConstIterator IndexMap<T>::cend() const {
    return m_data.cend();
}

// ===============================================================================
// FREE LIST
// ===============================================================================
template <typename T>
inline void IndexMap<T>::push_free_index( const Index& idx ) {
    std::deque<Index>::iterator free_it = std::lower_bound( m_free.begin(), m_free.end(), idx );
    m_free.insert( free_it, idx );
    if ( m_data.empty() ) {
        m_free.clear();
        m_free.push_back( Index( 0 ) );
    }
}

template <typename T>
inline bool IndexMap<T>::pop_free_index( Index& idx ) {
    if ( m_free.empty() ) {
        idx = Index::Invalid();
        return false;
    }
    idx = m_free.front();
    m_free.pop_front();
    if ( m_free.empty() ) {
        Index next = idx + 1;
        if ( next.isValid() ) {
            if ( uint( next.getValue() ) > m_data.size() ) { m_free.push_back( next ); }
        }
    }
    return true;
}

// ===============================================================================
// HELPER FUNCTIONS
// ===============================================================================

template <typename T>
typename IndexMap<T>::ConstIterator
IndexMap<T>::citfromIndex( const typename IndexMap<T>::ConstIndexIterator& it ) const {
    auto dataIt = m_data.begin();
    std::advance( dataIt, std::distance( m_index.cbegin(), it ) );
    return dataIt;
}

template <typename T>
typename IndexMap<T>::Iterator
IndexMap<T>::itfromIndex( const typename IndexMap<T>::ConstIndexIterator& it ) {
    auto dataIt = m_data.begin();
    std::advance( dataIt, std::distance( m_index.cbegin(), it ) );
    return dataIt;
}

template <typename T>
size_t IndexMap<T>::idxfromIndex( const typename IndexMap<T>::ConstIndexIterator& it ) const {
    return std::distance( m_index.cbegin(), it );
}

} // namespace Utils
} // namespace Core
} // namespace Ra
